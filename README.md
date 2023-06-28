# Space Hell

This project is an exploration of using LLMs to help or simplify programming a simple game. I'm mainly interested in evaluating the various tools available to assist in programming.

The wiki section is a repository of notes during the development.

You can compile and run this game yourself if you want. This project depends primarily on [chocolate](https://github.com/hsjunnesson/chocolate) which is a game framework I made. Make sure that submodule is up to date:

```
git submodule init
git submodule update
```

To get the dependencies working I suggest [vcpkg](https://github.com/microsoft/vcpkg). Install these following packages:

```
glfw3
glm
cjson
imgui[glfw-binding,opengl3-binding]
backward-cpp
```

Then use CMake to configure and build a solution.