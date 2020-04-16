Search Documents in Textpresso
==============================

Search documents indexed by Textpresso through queries on fulltext or sentences.

These are the APIs to perform document searches:

.. http:post:: /v1/textpresso/api/search_documents

   Search for documents indexed by Textpresso. **Requires authentication**

   :<json string token: a valid access token. See :doc:`obtaining_a_token` for further information on how to get one.
   :<json object query: a query object (see :doc:`query_object` for more details)
   :<json boolean include_fulltext: whether to return the fulltext and abstract of the documents.
                                    *Default value* is **false**. Restricted to specific tokens due to copyright.
   :<json boolean include_all_sentences: whether to return the text of all the sentences in the text.
                                          *Default value* is **false**. Restricted to specific tokens due to copyright.
   :<json boolean include_match_sentences: whether to return the text of each matched sentence. Valid only for sentence
                                     searches. *Default value* is **false**
   :<json int since_num: used for pagination. Skip the first results and return entries from the specified number. Note
                         that the counter starts from 0 - i.e., the first document is number 0.
   :<json int count: used for pagination. Return up to the specified number of results. *Maximum value* is **200**

   **Response Datatype Format**

   The returned data is a json array of objects, each of which contains the following fields:

   :>json string identifier: the document identifier
   :>json string score: the score of the document - an absolute number that indicates the degree to which the document
                        matches the provided query
   :>json string title: the title of the document
   :>json string author: the author(s) of the document
   :>json string accession: the accession of the document
   :>json string journal: the journal of the document
   :>json string year: publication year
   :>json string doc_type: the type of document (e.g., research article, review)
   :>json string fulltext: the fulltext of the document. Only if *include_fulltext* is set to **true** in the request.
   :>json string abstract: the abstract of the document. Only if *include_fulltext* is set to **true** in the request.
   :>jsonarr string all_sentences: the text of each sentence. Only if *include_all_sentences* is set to
                                       **true** in the request.
   :>jsonarr string matched_sentences: the text of each matched sentence. Only if *include_match_sentences* is set to
                                       **true** in the request and the query type is set to **sentence**.

   **Example request**:

   .. sourcecode:: http

      POST /v1/textpresso/api/search_documents HTTP/1.1
      Host: textpressocentral.org:18080
      Accept: application/json

      {
         "token": "123456789",
         "query": {
            "keywords": "DYN-1",
            "type": "document",
            "case_sensitive": false,
            "sort_by_year": false,
            "count": 2,
            "corpora": [
                          "C. elegans",
                          "C. elegans Supplementals"
                       ]
         }
      }

   **Example response**:

   .. sourcecode:: http

      HTTP/1.1 200 OK
      Vary: Accept
      Content-Type: text/javascript

      [
         {
            "doc_type": "Journal_article",
            "score": 0.0418161,
            "identifier": "I5m",
            "title": "Factors regulating the abundance and localization of synaptobrevin in the plasma membrane.",
            "author": "Dittman JS ; Kaplan JM",
            "accession": " Other:doi:10.1073\\/pnas.0600784103 PMID:16844789  WBPaper00027755",
            "journal": "Proc Natl Acad Sci U S A"
         },
         {
            "doc_type": "Journal_article",
            "score": 0.032331,
            "identifier": "B4r",
            "title": "A dynamin GTPase mutation causes a rapid and reversible temperature-inducible locomotion defect in C. elegans.",
            "author": "Clark SG ; Shurland D-L ; Meyerowitz EM ; Bargmann CI ; Van der Bliek AM",
            "accession": " Other:cgc2892 doi:10.1073\\/pnas.94.19.10438 PMID:9294229  WBPaper00002892",
            "journal": "Proc Natl Acad Sci U S A"
         }
      ]

   **Example request using Curl from the shell**

   .. code-block:: bash

      curl -k -d "{\"token\":\"XXXXX\", \"query\": {\"keywords\": \"yeast AND two AND hybrid\", \"year\": \"2017\", \"type\": \"sentence\", \"corpora\": [\"C. elegans\"]}, \"include_sentences\": true}" https://textpressocentral.org:18080/v1/textpresso/api/search_documents


.. http:post:: /v1/textpresso/api/get_documents_count

   Get the number of documents that match a search query. **Requires authentication**

   :<json string token: a valid access token. See :doc:`obtaining_a_token` for further information on how to get one.
   :<json object query: a query object (see :doc:`query_object` for more details)

   **Response Datatype Format**

   :>json int counter: the number of documents matching the query

   **Example request**:

   .. sourcecode:: http

      POST /v1/textpresso/api/get_documents_count HTTP/1.1
      Host: textpressocentral.org:18080
      Accept: application/json

      {
         "token": "123456789",
         "query": {
            "keywords": "DYN-1",
            "type": "document",
            "case_sensitive": false,
            "sort_by_year": false,
            "count": 2,
            "corpora": [
                          "C. elegans",
                          "C. elegans Supplementals"
                       ]
         }
      }

   **Example response**:

   .. sourcecode:: http

      HTTP/1.1 200 OK
      Vary: Accept
      Content-Type: text/javascript

      {
        "counter": 229
      }


.. http:get:: /v1/textpresso/api/available_corpora

   Get the list of corpora available on the server

   **Response Data Format**

   A json array of strings

   **Example request**:

   .. sourcecode:: http

      GET /v1/textpresso/api/available_corpora HTTP/1.1
      Host: textpressocentral.org:18080

   **Example response**:

   .. sourcecode:: http

      HTTP/1.1 200 OK
      Vary: Accept
      Content-Type: text/javascript

      ["C. elegans","C. elegans Supplementals","PMCOA C. elegans","PMCOA Animal"]

.. http:post:: /v1/textpresso/api/get_category_matches_document_fulltext

   Get the list of words in the fulltext of one or more documents that match a specified category. **Requires authentication**

   :<json string token: a valid access token. See :doc:`obtaining_a_token` for further information on how to get one.
   :<json object query: a query object used to search for the documents (see :doc:`query_object` for more details)
   :<json string category: a valid category in Textpresso format (e.g., "Gene (C. elegans) (tpgce:0000001)") - see
                           `Textpresso central category browser <https://www.textpressocentral.org/tpc/browsers>`_ for
                           the complete list of supported categories.

   **Response Datatype Format**

   The returned data is a json array of objects, each of which represents a document matched by the provided query, and
   contains the following fields:

   :>json string identifier: the document identifier
   :>jsonarr string matches: the list of words in the fulltext of the document that matched the specified category

   **Example request**:

   .. sourcecode:: http

      POST /v1/textpresso/api/get_category_matches_document_fulltext HTTP/1.1
      Host: textpressocentral.org:18080
      Accept: application/json

      {
         "token": "123456789",
         "query": {
            "accession": "WBPaper00050052",
            "corpora": [
                          "C. elegans",
                          "C. elegans Supplementals"
                       ]
         },
         "category": "Gene (C. elegans) (tpgce:0000001)"
      }

   **Example response**:

   .. sourcecode:: http

      HTTP/1.1 200 OK
      Vary: Accept
      Content-Type: text/javascript

      [
         {
            "identifier":"C. elegans/WBPaper00050052/WBPaper00050052.tpcas",
            "matches": ["apl-1","cdc-42","ceh-36","daf-16","glp-1","hsf-1","ins-33","lin-14","lin-4","mec-4","pmp-3","rab-3","snb-1"]
         }
      ]