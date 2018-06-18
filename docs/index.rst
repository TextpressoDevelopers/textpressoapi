Textpresso - RESTful API for Biological Curation and Text Mining
================================================================

Textpresso API provides functions to query the Textpresso database, a collection of annotated scientific articles for
biological curation. Articles can be searched by keywords, biological categories, and bibliographical information.
Keywords and categories can be searched in the whole text of the documents or in each sentence separately. A score is
assigned to each article in Textpresso database according to the match with the provided query, and the articles
returned by searches are sorted by their score. Scores for sentence searches is calculated as the sum of the scores for
sentences in each document.

The base endpoint of the API is
*https://textpressocentral.org:18080/textpresso/api/v1/request_name* where *request_name* is the API to call.


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
