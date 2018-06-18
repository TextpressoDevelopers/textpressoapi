Search Documents in Textpresso
==============================

Search documents indexed by Textpresso through queries on fulltext or sentences.

These are the APIs to perform document searches:

.. http:post:: /textpresso/api/1.0/search_documents

   Search for documents indexed by Textpresso. **Requires authentication**

   :<json string token: a valid access token. See :doc:`obtaining_a_token` for further information on how to get one.
   :<json object query: a query object (see :doc:`query_object` for more details)
   :<json boolean include_fulltext: whether to return the fulltext and abstract of the documents.
                                    *Default value* is **false**
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
   :>json string doc_type: the type of document (e.g., research article, review)
   :>json string fulltext: the fulltext of the document. Only if *include_fulltext* is set to **true** in the request.
   :>json string journal: the abstract of the document. Only if *include_fulltext* is set to **true** in the request.

   **Example request**:

   .. sourcecode:: http

      POST /textpresso/api/v1/search_documents HTTP/1.1
      Host: textpressocentral.org:18080
      Accept: application/json

      {
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


.. http:post:: /textpresso/api/v1/get_documents_count

   Get the number of documents that match a search query. **Requires authentication**

   :<json string token: a valid access token. See :doc:`obtaining_a_token` for further information on how to get one.
   :<json object query: a query object (see :doc:`query_object` for more details)

   **Response Datatype Format**

   :>json int counter: the number of documents matching the query

   **Example request**:

   .. sourcecode:: http

      POST /textpresso/api/v1/search_documents HTTP/1.1
      Host: textpressocentral.org:18080
      Accept: application/json

      {
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

   **Example response**:

   .. sourcecode:: http

      HTTP/1.1 200 OK
      Vary: Accept
      Content-Type: text/javascript

      {
        "counter": 229
      }


.. http:get:: /textpresso/api/v1/available_corpora

   Get the list of corpora available on the server

   **Response Data Format**

   A json array of strings

   **Example request**:

   .. sourcecode:: http

      GET /textpresso/api/v1/available_corpora HTTP/1.1
      Host: textpressocentral.org:18080

   **Example response**:

   .. sourcecode:: http

      HTTP/1.1 200 OK
      Vary: Accept
      Content-Type: text/javascript

      ["C. elegans","C. elegans Supplementals","PMCOA C. elegans","PMCOA Animal"]
