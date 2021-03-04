## What is it?

A simple project solving test task. Written in C.


## Prerequisites

To build this project the following prerequisites are required:
- QtCreator (to open .pro file)
- gcc compiler

## Functionality

This app solve following task (description in Russian):

Написать TCP-сервер, который:
Принимает последовательность байт неограниченного размера,
Из всей принятой последовательности вычленяет субпоследовательность от символа 'S' до символа 'E'
Считает количество символов в субпоследовательности (без 'S' и 'E')
Отправляет это количество в текстовом виде обратно клиенту.
Требования:
Отсутствие внешних зависимостей - нельзя использовать сторонние реализации сервера (Boost.Asio/Asio, ACE, POCO etc)
Пример:
Отправлено           : AAAS000EBBB<cr><lf>
Субпоследовательность: S000E
Ответ сервера        : 3
