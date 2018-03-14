Query Object
============

Specify a query to search documents in textpresso

Calls that require a query object must contain the following fields:

.. http:any:: query

   :<json string keywords: *(optional)* the keywords to match in the text. Can contain logical operators AND and OR and
                           grouping by round brackets
   :<json string exclude_keywords: *(optional)* the keywords to exclude. Can contain logical operators AND and OR and
                           grouping by round brackets
   :<json string year: *(optional)* year of publication of the paper
   :<json string author: *(optional)* the author(s) of the paper
   :<json string accession: *(optional)* the accession of the paper
   :<json string journal: *(optional)* the journal where the paper has been published
   :<json string paper_type: *(optional)* the type of paper (e.g., research_article, review)
   :<json bool exact_match_author: *(optional)* apply exact match on the author field
   :<json bool exact_match_journal: *(optional)* apply exact match on the journal field
   :<json bool categories_and_ed: *(optional)* use AND logical operator between the provided categories
   :<jsonarr string categories: *(optional)* a set of categories to match in the text
   :<jsonarr string corpora: *(optional)* restrict the search to the specified list of corpora
   :<json string type: the type of search to perform. Accepted values are: **document** to query the fulltext of
                       documents and **sentence** to search in each sentence separately. *Default value* is **document**
   :<json boolean case_sensitive: whether to perform a case sensitive search. *Default value* is **false**
   :<json boolean sort_by_year: whether the results have to be sorted by publication date. *Default value* is **false**

**Example**:

.. code-block:: json

   {
     "query":
     {
       "keywords": "DYN-1",
       "type": "document",
       "case_sensitive": false,
       "sort_by_year": false,
       "corpora": [
                     "C. elegans",
                     "C. elegans Supplementals"
                  ]
     }
   }