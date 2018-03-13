#define CROW_ENABLE_SSL

#include <iostream>
#include "crow_all.h"
#include <textpresso/IndexManager.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include <boost/program_options.hpp>

using namespace tpc::index;
using namespace std;
using namespace SQLite;
using namespace boost::filesystem;

namespace po = boost::program_options;


bool is_token_valid(const string& db_path, const string& token) {
    try {
        SQLite::Database db(db_path);
        SQLite::Statement query(db, "SELECT * FROM tokens WHERE token = ?");
        query.bind(1, token);
        query.executeStep();
        return query.hasRow();
    }
    catch (std::exception& e) {
        std::cout << "exception: " << e.what() << std::endl;
    }
}

tpc::index::Query get_query(const crow::json::rvalue& json_req, const IndexManager& indexManager) {
    tpc::index::Query query;
    if (!json_req.has("query")) {
        throw runtime_error("query not specified");
    }
    auto json_query = json_req["query"];
    if (json_query.has("keywords"))
        query.keyword = json_query["keywords"].s();
    if (json_query.has("categories") && json_query["categories"].size() > 0) {
        for (auto& category : json_query["categories"]) {
            query.categories.push_back(category.s());
        }
    }
    if (json_query.has("type") && (json_query["type"].s() == "document" ||
                                 json_query["type"].s() == "sentence")) {
        string type = json_query["type"].s();
        if (type == "document") {
            query.type = QueryType::document;
        } else if (type == "sentence") {
            query.type = QueryType::sentence;
        }
    } else {
        throw runtime_error("query type not specified");
    }
    if (json_query.has("case_sensitive")) {
        query.case_sensitive = json_query["case_sensitive"].b();
    }
    if (json_query.has("sort_by_year")) {
        query.sort_by_year = json_query["sort_by_year"].b();
    }
    if (json_query.has("corpora") && json_query["corpora"].size() > 0) {
        for (auto& corpus : json_query["corpora"]) {
            query.literatures.push_back(corpus.s());
        }
    } else {
        query.literatures = indexManager.get_available_corpora();
    }
    return query;
}

int main(int argc, const char* argv[]) {
    po::options_description desc("options");
    po::positional_options_description p;
    po::variables_map vm;

    // arguments
    string ssl_cert;
    string ssl_key;
    string index_path;
    string login_database;

    try {
        desc.add_options()
                ("help,h", "produce help message")
                ("index,i", po::value<string>(&index_path)->default_value("/usr/local/textpresso/luceneindex"),
                 "textpresso index")
                ("login-database,d", po::value<string>(&login_database)->required(),
                 "textpresso index")
                ("ssl_cert,c", po::value<string>(&ssl_cert)->default_value(""),
                 "ssl certificate file")
                ("ssl_key,k", po::value<string>(&ssl_key)->default_value(""),
                 "ssl key file");
        po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
        po::notify(vm);

        if (vm.count("help")) {
            cout << desc << endl;
            return 1;
        }
    } catch (std::exception &e) {
        if (vm.count("help")) {
            cout << desc << endl;
            return (EXIT_SUCCESS);
        }
        std::cerr << "Error: " << e.what() << "\n";
        return (EXIT_FAILURE);
    }

    IndexManager indexManager(index_path);
    crow::SimpleApp app;

    CROW_ROUTE(app, "/v1/textpresso/api/search_documents")
            .methods("POST"_method)
            ([&indexManager, &login_database](const crow::request& req){
                // parse request
                auto json_req = crow::json::load(req.body);
                if (!json_req)
                    return crow::response(400);
                if (!json_req.has("token")) {
                    return crow::response(401);
                }
                if (!is_token_valid(login_database, json_req["token"].s())) {
                    return crow::response(401);
                }
                int64_t since_num(0);
                int64_t count(200);
                if (json_req.has("since_num"))
                    since_num = json_req["since_num"].i();
                if (json_req.has("count") && 0 < json_req["count"].i() < 200)
                    count = json_req["count"].i();
                bool include_text(false);
                if (json_req.has("include_fulltext"))
                    include_text = json_req["include_fulltext"].b();
                tpc::index::Query query;
                try {
                     query = get_query(json_req, indexManager);
                } catch (const runtime_error& e) {
                    return crow::response(400);
                }
                // call textpresso library
                SearchResults results = indexManager.search_documents(query);
                auto first_iter = results.hit_documents.begin() + since_num;
                if (first_iter > results.hit_documents.end()) {
                    first_iter = results.hit_documents.end();
                }
                auto last_iter = results.hit_documents.begin() + since_num + count;
                if (last_iter > results.hit_documents.end()) {
                    last_iter = results.hit_documents.end();
                }
                set<string> exclude_doc_fields = {"fulltext_compressed", "abstract_compressed",
                                                     "fulltext_cat_compressed"};
                if (include_text) {
                    exclude_doc_fields = {};
                }
                auto doc_details = indexManager.get_documents_details(
                        vector<tpc::index::DocumentSummary>(first_iter, last_iter), query.sort_by_year, false,
                        tpc::index::DOCUMENTS_FIELDS_DETAILED, {}, exclude_doc_fields);
                // response
                crow::json::wvalue json_resp;
                for (int i = 0; i < doc_details.size(); ++i) {
                    json_resp[i]["identifier"] = doc_details[i].identifier;
                    json_resp[i]["score"] = doc_details[i].score;
                    json_resp[i]["title"] =
                            doc_details[i].title.substr(6, doc_details[i].title.length() - 10);
                    json_resp[i]["author"] =
                            doc_details[i].author.substr(6, doc_details[i].author.length() - 10);
                    json_resp[i]["accession"] = doc_details[i].accession;
                    json_resp[i]["journal"] =
                            doc_details[i].journal.substr(6, doc_details[i].journal.length() - 10);
                    json_resp[i]["doc_type"] = doc_details[i].type;
                    if (include_text) {
                        json_resp[i]["fulltext"] = doc_details[i].fulltext;
                        json_resp[i]["abstract"] = doc_details[i].abstract;
                    }
                }
                return crow::response(json_resp);
            });

    CROW_ROUTE(app, "/v1/textpresso/api/available_corpora")([&indexManager] {
        crow::json::wvalue x;
        auto corpora = indexManager.get_available_corpora();
        for (int i = 0; i < corpora.size(); ++i) {
            x[i] = corpora[i];
        }
        return crow::response(x);
    });

    CROW_ROUTE(app, "/v1/textpresso/api/get_documents_count")
            .methods("POST"_method)
                    ([&indexManager, &login_database](const crow::request& req){
                        // parse request
                        auto json_req = crow::json::load(req.body);
                        if (!json_req)
                            return crow::response(400);
                        if (!json_req.has("token")) {
                            return crow::response(400);
                        }
                        if (!is_token_valid(login_database, json_req["token"].s())) {
                            return crow::response(401);
                        }
                        tpc::index::Query query;
                        try {
                            query = get_query(json_req, indexManager);
                        } catch (const runtime_error& e) {
                            return crow::response(400);
                        }
                        // call textpresso library
                        SearchResults results = indexManager.search_documents(query);
                        // response
                        crow::json::wvalue json_resp;
                        json_resp = results.hit_documents.size();
                        return crow::response(json_resp);
                    });

    if (!ssl_cert.empty() && !ssl_key.empty()) {
        app.port(18080).ssl_file(ssl_cert, ssl_key).multithreaded().run();
    } else {
        app.port(18080).multithreaded().run();
    }
}