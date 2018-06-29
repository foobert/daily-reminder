const express = require("express");
const debug = require("debug")("dailyreminder:server");
const { check, calmDown } = require("./lib/timer");

const app = express();

// yes, we should probably persist this somewhere
// but for now resets at restart are okay
let lastButtonPush = null;

const resetTimes = [
  4 * 60, // 04:00 UTC
  16 * 60 // 16:00 UTC
];

function getState() {
  const now = new Date();
  const ok = check(now, lastButtonPush, resetTimes);
  if (ok) {
    if (calmDown(now, lastButtonPush)) {
      return "0\n";
    } else {
      return "2\n";
    }
  } else {
    return "1\n";
  }
}

app.get("/status", (req, res) => {
  res.status(200).send(getState());
});

app.post("/status", (req, res) => {
  lastButtonPush = new Date();
  res.status(200).send(getState());
});

app.listen(3000);
