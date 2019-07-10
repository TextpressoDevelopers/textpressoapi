#define CROW_ENABLE_SSL

#include <iostream>
#include "crow.h"
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

bool is_superuser(const string& db_path, const string& token) {
    try {
        SQLite::Database db(db_path);
        SQLite::Statement query(db, "SELECT superuser FROM tokens WHERE token = ?");
        query.bind(1, token);
        query.executeStep();
        if (query.hasRow()) {
            return static_cast<bool>(static_cast<int>(query.getColumn("superuser")));
        } else {
            return false;
        }
    }
    catch (std::exception& e) {
        std::cout << "exception: " << e.what() << std::endl;
    }
}

static bool sentence_before(const SentenceDetails &a, const SentenceDetails &b) {
    return a.doc_position_begin < b.doc_position_begin;
}

tpc::index::Query get_query(const crow::json::rvalue& json_req, const IndexManager& indexManager) {
    tpc::index::Query query;
    if (!json_req.has("query")) {
        throw runtime_error("query not specified");
    }
    if (json_req["query"].has("keywords"))
        query.keyword = json_req["query"]["keywords"].s();
    if (json_req["query"].has("categories") && json_req["query"]["categories"].size() > 0) {
        for (auto& category : json_req["query"]["categories"]) {
            query.categories.push_back(category.s());
        }
    }
    if (json_req["query"].has("exclude_keywords"))
        query.exclude_keyword = json_req["query"]["exclude_keywords"].s();
    if (json_req["query"].has("year"))
        query.year = json_req["query"]["year"].s();
    if (json_req["query"].has("author"))
        query.author = json_req["query"]["author"].s();
    if (json_req["query"].has("accession"))
        query.accession = json_req["query"]["accession"].s();
    if (json_req["query"].has("journal"))
        query.journal = json_req["query"]["journal"].s();
    if (json_req["query"].has("paper_type"))
        query.paper_type = json_req["query"]["paper_type"].s();
    if (json_req["query"].has("exclude_keywords"))
        query.exclude_keyword = json_req["query"]["exclude_keywords"].s();
    if (json_req["query"].has("exact_match_author"))
        query.exact_match_author = json_req["query"]["exact_match_author"].b();
    if (json_req["query"].has("exact_match_journal"))
        query.exact_match_journal = json_req["query"]["exact_match_journal"].b();
    if (json_req["query"].has("categories_and_ed"))
        query.categories_and_ed = json_req["query"]["categories_and_ed"].b();
    if (json_req["query"].has("type") && (json_req["query"]["type"].s() == "document" ||
                                 json_req["query"]["type"].s() == "sentence")) {
        string type = json_req["query"]["type"].s();
        if (type == "document") {
            query.type = QueryType::document;
        } else if (type == "sentence") {
            query.type = QueryType::sentence;
        }
    } else {
        throw runtime_error("query type not specified");
    }
    if (json_req["query"].has("case_sensitive")) {
        query.case_sensitive = json_req["query"]["case_sensitive"].b();
    }
    if (json_req["query"].has("sort_by_year")) {
        query.sort_by_year = json_req["query"]["sort_by_year"].b();
    }
    if (json_req["query"].has("corpora") && json_req["query"]["corpora"].size() > 0) {
        for (auto& corpus : json_req["query"]["corpora"]) {
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
                if (json_req.has("count") && 0 < json_req["count"].i() && json_req["count"].i() < 200)
                    count = json_req["count"].i();
                bool include_text(false);
                bool include_match_sentences(false);
                bool include_all_sentences(false);
                if (json_req.has("include_fulltext")) {
                    if (is_superuser(login_database, json_req["token"].s())) {
                        include_text = json_req["include_fulltext"].b();
                    } else {
                        return crow::response(401);
                    }
                }
                tpc::index::Query query;
                try {
                     query = get_query(json_req, indexManager);
                } catch (const runtime_error& e) {
                    cerr << e.what() << endl;
                    return crow::response(400);
                }
                set<string> include_match_sentence_fields = {};
                if (json_req.has("include_match_sentences")) {
                    include_match_sentences = json_req["include_match_sentences"].b();
                    include_match_sentence_fields = {"sentence_compressed", "begin"};
                    if (include_match_sentences && query.type == tpc::index::QueryType::document) {
                        return crow::response(401);
                    }
                }
                set<string> include_all_sentence_fields = {};
                if (json_req.has("include_all_sentences")) {
                    if (is_superuser(login_database, json_req["token"].s())) {
                        include_all_sentences = json_req["include_all_sentences"].b();
                        include_all_sentence_fields = {"sentence_compressed", "begin"};
                    } else {
                        return crow::response(401);
                    }
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
                        vector<tpc::index::DocumentSummary>(first_iter, last_iter), query.sort_by_year,
                        include_match_sentences, tpc::index::DOCUMENTS_FIELDS_DETAILED, include_match_sentence_fields,
                        exclude_doc_fields, {}, include_all_sentences, include_all_sentence_fields, {}, true, true);
                // response
                crow::json::wvalue json_resp;
                for (int i = 0; i < doc_details.size(); ++i) {
                    json_resp[i]["identifier"] = doc_details[i].filepath;
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
                    if (include_match_sentences) {
                        sort(doc_details[i].sentences_details.begin(),
                                doc_details[i].sentences_details.end(), sentence_before);
                        for (int j = 0; j < doc_details[i].sentences_details.size(); ++j) {
                            json_resp[i]["matched_sentences"][j] = doc_details[i].sentences_details[j].sentence_text;
                        }
                    }
                    if (include_all_sentences) {
                        sort(doc_details[i].all_sentences_details.begin(), doc_details[i].all_sentences_details.end(),
                             sentence_before);
                        for (int j = 0; j < doc_details[i].all_sentences_details.size(); ++j) {
                            json_resp[i]["all_sentences"][j] = doc_details[i].all_sentences_details[j].sentence_text;
                        }
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

    CROW_ROUTE(app, "/v1/textpresso/api/get_category_matches_document_fulltext")
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
                        if (!json_req.has("category")) {
                            return crow::response(400);
                        }
                        string category = json_req["category"].s();
                        // response
                        crow::json::wvalue json_resp;
                        // call textpresso library
                        std::set<std::string> matches;
                        SearchResults results = indexManager.search_documents(query);
                        int i = 0;
                        for (auto& result : results.hit_documents) {
                            DocumentDetails docDetails = indexManager.get_document_details(result, false,
                                    {"filepath", "fulltext_compressed", "fulltext_cat_compressed"}, {}, {}, {});
                            json_resp[i]["identifier"] = docDetails.filepath;
                            matches = indexManager.get_words_belonging_to_category_from_document_fulltext(
                                    docDetails.fulltext, docDetails.categories_string, category);
                            int j = 0;
                            for (auto& word : matches) {
                                json_resp[i]["matches"][j++] = word;
                            }
                            ++i;
                        }
                        return crow::response(json_resp);
                    });

    if (!ssl_cert.empty() && !ssl_key.empty()) {
        auto & appref = app
                .bindaddr("0.0.0.0")
                .port(18080)
                .ssl_file(ssl_cert, ssl_key);
        appref.ssl_context_.set_verify_mode (boost::asio::ssl::verify_none);
        appref.multithreaded().run();
    } else {
        app.port(18080).multithreaded().run();
    }
}