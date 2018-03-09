#include <iostream>
#include "crow_all.h"
#include <textpresso/IndexManager.h>

using namespace tpc::index;
using namespace std;

int main() {
    static const string index_root_dir = "/usr/local/textpresso/luceneindex";
    IndexManager indexManager(index_root_dir);

    crow::SimpleApp app;

    CROW_ROUTE(app, "/v1/textpresso/api/search_documents")
            .methods("POST"_method)
            ([&indexManager](const crow::request& req){
                // parse request
                auto json_req = crow::json::load(req.body);
                tpc::index::Query query = tpc::index::Query();
                if (!json_req)
                    return crow::response(400);
                int64_t since_num(0);
                int64_t count(200);
                if (json_req.has("since_num"))
                    since_num = json_req["since_num"].i();
                if (json_req.has("count") && 0 < json_req["count"].i() < 200)
                    count = json_req["count"].i();
                bool include_text(false);
                if (json_req.has("include_text"))
                    include_text = json_req["include_text"].b();
                if (json_req.has("keywords"))
                    query.keyword = json_req["keywords"].s();
                if (json_req.has("categories") && json_req["categories"].size() > 0) {
                    for (auto& category : json_req["categories"]) {
                        query.categories.push_back(category.s());
                    }
                }
                if (json_req.has("type") && (json_req["type"].s() == "document" ||
                        json_req["type"].s() == "sentence")) {
                    string type = json_req["type"].s();
                    if (type == "document") {
                        query.type = QueryType::document;
                    } else if (type == "sentence") {
                        query.type = QueryType::sentence;
                    }
                } else {
                    return crow::response(501);
                }
                if (json_req.has("case_sensitive")) {
                    query.case_sensitive = json_req["case_sensitive"].b();
                }
                if (json_req.has("sort_by_year")) {
                    query.sort_by_year = json_req["sort_by_year"].b();
                }
                if (json_req.has("corpora") && json_req["corpora"].size() > 0) {
                    for (auto& corpus : json_req["corpora"]) {
                        query.literatures.push_back(corpus.s());
                    }
                } else {
                    query.literatures = indexManager.get_available_corpora();
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
                    json_resp["hit_documents"][i]["identifier"] = doc_details[i].identifier;
                    json_resp["hit_documents"][i]["score"] = doc_details[i].score;
                    json_resp["hit_documents"][i]["title"] =
                            doc_details[i].title.substr(6, doc_details[i].title.length() - 10);
                    json_resp["hit_documents"][i]["author"] =
                            doc_details[i].author.substr(6, doc_details[i].author.length() - 10);
                    json_resp["hit_documents"][i]["accession"] = doc_details[i].accession;
                    json_resp["hit_documents"][i]["journal"] =
                            doc_details[i].journal.substr(6, doc_details[i].journal.length() - 10);
                    json_resp["hit_documents"][i]["doc_type"] = doc_details[i].type;
                    if (include_text) {
                        json_resp["hit_documents"][i]["fulltext"] = doc_details[i].fulltext;
                        json_resp["hit_documents"][i]["abstract"] = doc_details[i].abstract;
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

    CROW_ROUTE(app, "/v1/textpresso/api/available_doc_fields")([]() {
        crow::json::wvalue x;
        int i = 0;
        for (const auto& field : tpc::index::DOCUMENTS_FIELDS_DETAILED) {
            x[i++] = field;
        }
        return crow::response(x);
    });

    CROW_ROUTE(app, "/v1/textpresso/api/available_sentence_fields")([]() {
        crow::json::wvalue x;
        int i = 0;
        for (const auto& field : tpc::index::SENTENCE_FIELDS_DETAILED) {
            x[i++] = field;
        }
        return crow::response(x);
    });

    app.port(18080).multithreaded().run();
}