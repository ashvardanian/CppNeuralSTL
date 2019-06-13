# NeuralSTL

This repo contains the presentation and demos code for my talk ["AI & Computational Graphs in C++"](Data/Presentation.pdf).
It has a couple of simple MLP implementations and a much more complicated, but incomplete example.
The purpose of the talk is to: 
- cover the challenges of building end-to-end AI systems.
- convience listeners that engineering reliable & scalable software for neural networks is at least as complicated as most of the modern AI research.

The endgoal is to use the means of C++ to build an AI model capable of estimating the code quality based on:
- AST tree extracted from Clang,
- commits history extracted from GIT,
- language features frequency of use.

That data will be used to predict features (like the number of forks or contributors) of various open-source projects.

To train an accurate model we need a large amount of data.
I have limited myself to the top ~800 most popular C++ libraries and ~700 other popular, but small libraries. The full list of included libraries is availiable [here](Data/stats_github.csv).
Those libraries together a have a total of ~600K source files. 

Don't hesitate to contact me if you want to help or have any suggestions!
