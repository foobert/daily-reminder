const express = require("express");
const debug = require("debug")("lolin:server");

const app = express();

let s = 0;

app.get("/status", (req, res) => {
  debug("got status request");
  res.status(200).send("" + s + "\n");
});

app.post("/status", (req, res) => {
  if (s == 0) {
    s = 1;
  } else {
    s = 0;
  }
  res.status(200).send("" + s + "\n");
});

app.listen(3000);
