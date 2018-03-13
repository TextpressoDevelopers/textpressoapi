Textpresso - RESTful API for Biological Curation and Text Mining
================================================================

Textpresso API provides functions to query the Textpresso database, a collection of annotated scientific articles for
biological curation. Articles can be searched by keywords, biological categories, and bibliographical information.
Keywords and categories can be searched in the whole text of the documents or in each sentence separately. A score is
assigned to each article in Textpresso database according to the match with the provided query, and the articles
returned by searches are sorted by their score. Scores for sentence searches is calculated as the sum of the scores for
sentences in each document.

Since searches might yield a large number of documents, they only return summary information of the articles in order to
obtain better performances. To read the detailed information contained the articles obtained by a search, Textpresso
API limits the number of documents that can be retrieved by each call to 200 and provides parameters for pagination and
to explicitly control the output of fields with potentially large content (e.g., document full text).

The base endpoint of the API is
*http://localhost:18080/textpresso/api/1.0/request_name* where *request_name* is the API to call.


.. toctree::
   :maxdepth: 2
   :caption: Contents:

   search_documents
   query_object
   obtaining_a_token


Indices and tables
==================

* :ref:`genindex`
* :ref:`search`
