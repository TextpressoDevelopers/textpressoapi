#include <iostream>
#include "crow_all.h"
#include <textpresso/IndexManager.h>

using namespace tpc::index;
using namespace std;

int main() {
    static const string index_root_dir = "/usr/local/textpresso/luceneindex";
    string line;
    map<string, vector<string>> subindices_map;
    vector<string> all_literatures;
    ifstream lit_file (index_root_dir + "/subindex.config");
    if (lit_file.is_open()) {
        while ( getline (lit_file, line) ) {
            subindices_map[line.substr(0, line.find_first_of('_'))].push_back(line);
            all_literatures.push_back(line.substr(0, line.find_first_of('_')));
        }
        lit_file.close();
    } else {
        cerr << "unable to read subindex.config file" << endl;
    }

    IndexManager indexManager(index_root_dir);

    crow::SimpleApp app;

    CROW_ROUTE(app, "/v1/textpresso/api/search_documents")
            .methods("POST"_method)
            ([&subindices_map, &all_literatures, &indexManager](const crow::request& req){
                // parse request
                auto json_req = crow::json::load(req.body);
                tpc::index::Query query = tpc::index::Query();
                if (!json_req)
                    return crow::response(400);
                query.query_text = json_req["query"].s();
                string type = json_req["type"].s();
                if (type == "document") {
                    query.type = QueryType::document;
                } else if (type == "sentence_without_ids") {
                    query.type = QueryType::sentence_without_ids;
                } else if (type == "sentence_with_ids") {
                    query.type = QueryType::sentence_with_ids;
                }
                query.case_sensitive = json_req["case_sensitive"].b();
                query.sort_by_year = json_req["sort_by_year"].b();
                if (json_req.has("literatures") && json_req["literatures"].size() > 0) {
                    for (const auto& literature : json_req["literatures"]) {
                        for (const string& subindex : subindices_map[literature.s()]) {
                            query.literatures.push_back(subindex);
                        }
                    }
                } else {
                    query.literatures = all_literatures;
                }
                // call textpresso library
                SearchResults results = indexManager.search_documents(query);
                // response
                crow::json::wvalue json_resp;
                for (int i = 0; i < results.hit_documents.size(); ++i) {
                    json_resp["hit_documents"][i]["identifier"] = results.hit_documents[i].identifier;
                    json_resp["hit_documents"][i]["score"] = results.hit_documents[i].score;
                    if (query.type == QueryType::sentence_with_ids) {
                        for (int j = 0; j < results.hit_documents[i].matching_sentences.size(); ++j) {
                            json_resp["hit_documents"][i]["matching_sentences"][j]["sentence_id"] =
                                    results.hit_documents[i].matching_sentences[j].sentence_id;
                            json_resp["hit_documents"][i]["matching_sentences"][j]["score"] =
                                    results.hit_documents[i].matching_sentences[j].score;
                        }
                    }
                    json_resp["total_num_sentences"] = results.total_num_sentences;
                    json_resp["min_score"] = results.min_score;
                    json_resp["max_score"] = results.max_score;
                }
                return crow::response(json_resp);
            });

    CROW_ROUTE(app, "/v1/textpresso/api/get_documents_details")
            .methods("POST"_method)
                    ([&all_literatures, &indexManager](const crow::request& req){
                        // parse request
                        auto json_req = crow::json::load(req.body);
                        vector<string> sub_lit = all_literatures;
                        vector<DocumentSummary> documentSummaries;
                        set<string> include_doc_fields;
                        set<string> include_sentence_fields;
                        set<string> exclude_doc_fields = {};
                        set<string> exclude_sentence_fields = {};
                        if (!json_req)
                            return crow::response(400);
                        for (const auto& doc : json_req["docs_to_get"]) {
                            DocumentSummary documentSummary = DocumentSummary();
                            documentSummary.identifier = doc["identifier"].s();
                            for (const auto& sentence : doc["sentences_to_read"]) {
                                SentenceSummary sentSummary = SentenceSummary();
                                sentSummary.sentence_id = static_cast<int>(sentence.i());
                                documentSummary.matching_sentences.push_back(sentSummary);
                            }
                            documentSummaries.push_back(documentSummary);
                        }
                        bool include_sentences_details = json_req["include_sentences_details"].b();
                        if (json_req.has("include_doc_fields") && json_req["include_doc_fields"].size() > 0) {
                            for (const auto& field : json_req["include_doc_fields"]) {
                                include_doc_fields.insert(field.s());
                            }
                        } else {
                            include_doc_fields = tpc::index::document_fields_detailed;
                        }
                        if (json_req.has("include_sentence_fields") && json_req["include_sentence_fields"].size() > 0) {
                            for (const auto& field : json_req["include_sentence_fields"]) {
                                include_sentence_fields.insert(field.s());
                            }
                        } else {
                            include_sentence_fields = tpc::index::sentence_fields_detailed;
                        }
                        if (json_req.has("exclude_doc_fields") && json_req["exclude_doc_fields"].size() > 0) {
                            for (const auto& field : json_req["exclude_doc_fields"]) {
                                exclude_doc_fields.insert(field.s());
                            }
                        }
                        if (json_req.has("exclude_sentence_fields") && json_req["exclude_sentence_fields"].size() > 0) {
                            for (const auto& field : json_req["exclude_sentence_fields"]) {
                                exclude_sentence_fields.insert(field.s());
                            }
                        }
                        // call textpresso library
                        vector<DocumentDetails> results = indexManager.get_documents_details(
                                documentSummaries, sub_lit, false, include_sentences_details,
                                include_doc_fields, include_sentence_fields, exclude_doc_fields,
                                exclude_sentence_fields);
                        // response
                        crow::json::wvalue json_resp;
                        for (int i = 0; i < results.size(); ++i) {
                            json_resp[i]["filepath"] = results[i].filepath;
                            json_resp[i]["fulltext"] = results[i].fulltext;
                            json_resp[i]["categories_string"] = results[i].categories_string;
                            json_resp[i]["abstract"] = results[i].abstract;
                            json_resp[i]["literature"] = results[i].literature;
                            json_resp[i]["accession"] = results[i].accession;
                            json_resp[i]["title"] = results[i].title;
                            json_resp[i]["author"] = results[i].author;
                            json_resp[i]["journal"] = results[i].journal;
                        }
                        return crow::response(json_resp);
                    });

    CROW_ROUTE(app, "/v1/textpresso/api/available_literatures")([&all_literatures]() {
        crow::json::wvalue x;
        for (int i = 0; i < all_literatures.size(); ++i) {
            x[i] = all_literatures[i];
        }
        return crow::response(x);
    });

    CROW_ROUTE(app, "/v1/textpresso/api/available_doc_fields")([]() {
        crow::json::wvalue x;
        int i = 0;
        for (const auto& field : tpc::index::document_fields_detailed) {
            x[i++] = field;
        }
        return crow::response(x);
    });

    CROW_ROUTE(app, "/v1/textpresso/api/available_sentence_fields")([]() {
        crow::json::wvalue x;
        int i = 0;
        for (const auto& field : tpc::index::sentence_fields_detailed) {
            x[i++] = field;
        }
        return crow::response(x);
    });

    app.port(18080).multithreaded().run();
}