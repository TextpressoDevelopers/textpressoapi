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
 API provides a set of dedicated functions.

In addition to searching articles, Textpresso API provides functions to add new pdf or xml documents to the
database. The text is extracted from documents added to the system and divided into sentences. Then, the documents are
classified into different corpora according to their content and the the biological categories are automatically
annotated. The resulting information is stored in Textpresso and indexed for searches.

Textpresso API provides also functionalities to add biological categories.

The base endpoint of the API is
*http://localhost:18080/textpresso/api/1.0/request_name* where *request_name* is the request to call.


.. toctree::
   :maxdepth: 2
   :caption: Contents:

   search_documents
   add_documents
   categories



Indices and tables
==================

* :ref:`genindex`
* :ref:`search`
