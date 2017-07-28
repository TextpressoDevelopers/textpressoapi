Search Documents in Textpresso
==============================

Search for documents indexed by Textpresso through queries on fulltext or sentences.

These are the APIs to perform document searches:

.. http:post:: /textpresso/api/1.0/search_documents

   Search for documents indexed by Textpresso

   :<json string query: the query text
   :<json string type: the type of search to perform. Accepted values are: **document**, **sentence_without_ids**, and
                       **sentence_with_ids**
   :<json boolean case_sensitive: whether to perform a case sensitive search. *Default value* = **false**
   :<json boolean sort_by_year: whether the results have to be sorted by publication date. *Default value* = **false**
   :<json jsonarr literatures: the list of literatures to search. *Default value* = search all literatures

   **Response Datatype Format**

   The returned json object reflects the structure of the SearchResult object defined in textpressocentral C++ library.

   **Example request**:

   .. sourcecode:: http

      POST /textpresso/api/1.0/search_documents HTTP/1.1
      Host: localhost:18080
      Accept: application/json

      {
         "query":"sentence:DYN-1",
         "type":"sentence_without_ids",
         "case_sensitive":false,
         "sort_by_year":false
         "literatures":[
                          "C. elegans",
                          "C. elegans Supplementals"
                       ]
      }

   **Example response**:

   .. sourcecode:: http

      HTTP/1.1 200 OK
      Vary: Accept
      Content-Type: text/javascript

      {
         "max_score":2930.69,
         "min_score":10.1792,
         "total_num_sentences":2,
         "hit_documents":[
                            {
                               "score":2930.69,
                               "identifier":"eec99f625bd88d56"
                            },
                            {
                               "score":2669.16,
                               "identifier":"0c462283ed4ac35e"
                            }
                         ]
      }


.. http:post:: /textpresso/api/1.0/get_documents_details

   Get detailed information about documents and, optionally, about their sentences

   :<json vector docs_to_get: the list of documents to search with their identifiers and the sentences for which
                                          to retrieve detailed information
   :<json boolean include_sentences_details: whether to retrieve detailed information for the sentences of the documents
                                                         specified in the **doc_to_get** object. *Default value* = **false**
   :<json vector include_doc_fields: the list of fields to retrieve for the documents. **Default value** = all fields
   :<json vector include_sentence_fields: the list of fields to retrieve for the sentences. **Default value** = all
                                          fields
   :<json vector exclude_doc_fields: the list of document fields to exclude from the results. **Default value** =
                                     **none**
   :<json vector exclude_sentence_fields: the list of sentence fields to exclude from the results. **Default value** =
                                          **none**

   **Response Data Format**

   The returned json vector contains objects that reflect the structure of the DocumentDetails object defined in
   textpressocentral C++ library.

   **Example request**:

   .. sourcecode:: http

      POST /textpresso/api/1.0/get_documents_details HTTP/1.1
      Host: localhost:18080
      Accept: application/json

      {
         "docs_to_get":[
                          {
                             "identifier":"eec99f625bd88d56",
                             "sentences_to_read":[1,2,3]
                          }
                       ],
         "include_sentences_details":true,
         "exclude_doc_fields":["fulltext_compressed"],
         "exclude_doc_fields":["fulltext_cat_compressed"]
      }

   **Example response**:

   .. sourcecode:: http

      HTTP/1.1 200 OK
      Vary: Accept
      Content-Type: text/javascript

      [
         {
            "author":"BEGIN He Bin, Yu Xiaomeng, Margolis Moran, Liu Xianghua, Leng Xiaohong, Etzion Yael, Zheng Fei, Lu Nan, Quiocho Florante A., Danino Dganit, Zhou Zheng, Shaw Janet M. END",
            "journal":"BEGIN Molecular Biology of the Cell END",
            "title":"BEGIN      Live-Cell Imaging in      <named-content content-type=\"genus-species\">Caenorhabditis elegans</named-content>      Reveals the Distinct Roles of Dynamin Self-Assembly and Guanosine Triphosphate Hydrolysis in the Removal of Apoptotic Cells     END","filepath":"PMCOA C. elegans/Mol_Biol_Cell_2010_Feb_15_21(4)_610-629/zmk610.tpcas","accession":"PMID       20016007",
            "fulltext":"",
            "categories_string":"",
            "literature":"PMCOA C. elegans",
            "abstract":"    <p>During cell corpse removal, dynamin's self-assembly and GTP hydrolysis activities establish a precise dynamic control of DYN-1's transient association to its target membranes. Dynamin's dynamic membrane association controls the mechanism that underlies the recruitment of downstream effectors, such as small GTPases RAB-5 and RAB-7, to target membranes.</p>   "
         }
      ]


.. http:get:: /textpresso/api/1.0/available_literatures

   Get the list of literatures available on the server

   **Response Data Format**

   A json array of strings

   **Example request**:

   .. sourcecode:: http

      GET /textpresso/api/1.0/available_literatures HTTP/1.1
      Host: localhost:18080

   **Example response**:

   .. sourcecode:: http

      HTTP/1.1 200 OK
      Vary: Accept
      Content-Type: text/javascript

      ["C. elegans","C. elegans Supplementals","PMCOA C. elegans","PMCOA Animal"]


.. http:get:: /textpresso/api/1.0/available_doc_fields

   Get the list of document fields available on the server

   **Response Data Format**

   A json array of strings

   **Example request**:

   .. sourcecode:: http

      GET /textpresso/api/1.0/available_doc_fields HTTP/1.1
      Host: localhost:18080

   **Example response**:

   .. sourcecode:: http

      HTTP/1.1 200 OK
      Vary: Accept
      Content-Type: text/javascript

      ["abstract_compressed","accession_compressed","author_compressed","filepath","fulltext_cat_compressed","fulltext_compressed","identifier","journal_compressed","literature_compressed","title_compressed","year"]


.. http:get:: /textpresso/api/1.0/available_sentence_fields

   Get the list of sentence fields available on the server

   **Response Data Format**

   A json array of strings

   **Example request**:

   .. sourcecode:: http

      GET /textpresso/api/1.0/available_doc_fields HTTP/1.1
      Host: localhost:18080

   **Example response**:

   .. sourcecode:: http

      HTTP/1.1 200 OK
      Vary: Accept
      Content-Type: text/javascript

      ["begin","end","sentence_cat_compressed","sentence_compressed","sentence_id"]
