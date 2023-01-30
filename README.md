# ReadsProfiler
  1. Introduction

  ReadsProfiler is an application that uses multithreading for processing a number of clients. It will create a profile for every user and he will be able to add to his personal library in different shelves ("Reading", "Read", "Dropped", "Planning to read") and other info (ex. a review). After more reviews, each user will have a profile on which he will get recommended books based on what he has already read.
  Also, you can search for a book (name or ISBN), author, genre or, as an admin, you can add books, authors, and any other info through an intuitive console. Another option is downloading the book on a specified folder (the default is Downloads). We do not condone any kind of piracy and such, the examples already on the server are just random words and letters.

  2. Used technologies

  For this type of application, TCP technology would be right because there won't be any lost info between client and server. These losses would modify the users' profiles in unwanted ways.
   For the transmitted data, the program uses threads for each process because a thread for each user would limit the number of users permitted on the site. Some threads, like the login and register ones, are combined because these are requests sent only once per sesson.
   Every user data and the databases are stored in XML files on the local machine.
