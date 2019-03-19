void static_index(Request &req, Response &res) {
  P(index) =
    "<html>\n"
    "<head>\n"
    "<title>Hello World!</title>\n"
    "</head>\n"
    "<body>\n"
    "<h1>Greetings middle earth!</h1>\n"
    "</body>\n"
    "</html>";

  res.set("Content-Type", "text/html");
  res.printP(index);
}

Router staticFileRouter("/");

Router * staticFiles() {
  staticFileRouter.get("", &static_index);
  return &staticFileRouter;
}
