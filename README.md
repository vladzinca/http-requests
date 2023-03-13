# HTTP Requests

## ⏩ What is it?

It's a web client written in C that interacts with a server in order to manage an Internet library.

In order to do this, it uses a [REST API](https://en.wikipedia.org/wiki/Representational_state_transfer) exposed by the server in order to send GET, POST and DELETE requests specific to HTTP.

I worked on this for two days during May 2022 and it's one of the projects I am the most proud of so far.

## 💻 How to run it?

1.  Clone this repository.
2.  Compile using `make` and open the client using `make run`. However, the functionalities could be limited because the server might not be open anymore.
3.  Enjoy!

## 🚀 How does it work?

The implemented functionalities comprise being able to register and log in to the site, entering the Internet library, printing the list of books from the library or information about a said book, adding and deleting books, as well as logging out and exiting the program.

They are detailed in what follows:

### register

This command can be used to register a user, and gives you a prompt for `username` and `password`.

```
register
username=string
password=string
```

It then sends a POST request to the URL `/api/v1/tema/auth/register` with the following payload:

```
{
    "username": String,
    "password": String
}
```

If the username is already in use, it returns an error.

### login

This command can be used to log in an existing user, and it offers you a prompt for `username` and `password`.

```
login
username=string
password=string
```

It then sends a POST request to the URL `/api/v1/tema/auth/login` with the following payload:

```
{
  "username": String,
  "password": String
}
```

And returns a [session cookie](https://en.wikipedia.org/wiki/HTTP_cookie), or an error message if the credentials don't match.

### enter_library

This command requests access to the server library.

```
enter_library
```

It sends a GET request to the URL `/api/v1/tema/library/access`, and returns a [JWT token](https://en.wikipedia.org/wiki/JSON_Web_Token) that demonstrates you have gained access to the library.

If you are not logged in, however, it returns an error.

### get_books

This command makes a request to the server for a list of the books available in the library.

```
get_books
```

It sends a GET request to the URL `/api/v1/tema1/library/books`, and returns a list of [JSON](https://en.wikipedia.org/wiki/JSON) objects like:

```
[
    {
        id: Number,
        title: String
    }
]
```

Or an error message, if you do not have access to the library.

### get_book

This command requests information about a specific book identifiable by `id` and gives you a prompt for typing out said id.

```
get_book
id=number
```

It then sends a GET request to the URL `/api/v1/tema/library/books/:bookId`, and returns a JSON object like:

```
{
    "id": Number,
    "title": String,
    "author": String,
    "publisher": String,
    "genre": String,
    "page_count": Number
}
```

Or an error message, if you do not have access to the library or if the id you have requested the book for is invalid.

### add_book

This command can be used to add a book to the libray and it offers a prompt for different details of the book, such as `title`, `author`, `genre`, `publisher` and `page_count`.

```add_book
title=string
author=string
genre=string
publisher=string
page_count=number
```

It then sends a POST request to the URL `/api/v1/tema/library/books` with the following payload:

```
{
    "title": String,
    "author": String,
    "genre": String,
    "page_count": Number,
    "publisher: String
}
```

And returns an error message if you do not have access to the library, or if the information you added is incomplete or does not respect the formatting (e.g. `page_count` is not a number).

### delete_book

This command removes a book, and gives a prompt to select the book by id.

```
delete_book
id=number
```

It then sends a DELETE request to the URL `/api/v1/tema/library/books/:bookId`, where `:bookId` is the id of said book.

If you do not have access to the server library, or if the id you want to send the request for is inexistent, the app returns an error message.

### logout

This command allows a user to log out.

```
logout
```

It sends a GET request to the URL `/api/v1/tema/auth/logout`.

In order to work and not return an error message, you have to be logged in.

### exit

This command exits the application.

```
exit
```

## 🎓 What did I learn?

First of all, I found the application to be kind of unique in the sense that it uses C to send HTTP requests to a server, which is something very different to most web clients out there (which of course use the classical HTML + CSS + JavaScript).

But this made it a more thrilling experience, as using a language like C can get you closer to the details of the protocol and facilitate a better understanding.

To me, finally implementing cookies and JWT's gave me a much more complete knowledge of the web, and the very blueprints of how it all works.

This HTTP project was the last piece of the puzzle that for me completed my understanding of the OSI model, and it made me so much more eager to broaden my knowledge of web programmming and how to develop real, life-like web applications.

## 🤔 Did you know?

I am very interested in how to write highly complex, yet fast applications on the web. As sites get more and more complex, I feel that the programming languages and frameworks we have at our disposal get more and more rudimentary.

If you're also interested, check out [WebAssembly](https://en.wikipedia.org/wiki/WebAssembly). Thank me later! 🧑‍🔧
