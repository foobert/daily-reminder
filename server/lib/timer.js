const debug = require("debug")("dailyreminder:check");
const {
  addMinutes,
  startOfDay,
  isSameDay,
  isWithinRange,
  subDays,
  differenceInMinutes
} = require("date-fns");

function check(now, lastButtonPush, resetTimes) {
  debug("check, now: %s, lastButtonPush: %s", now, lastButtonPush);
  if (!lastButtonPush) {
    debug("check failed, no button push");
    return false;
  }

  const today = startOfDay(now);
  const yesterday = subDays(today, 1);

  const resets = resetTimes.map(r => [
    addMinutes(today, r),
    addMinutes(yesterday, r)
  ]);
  const anyReset = flatten(resets).some(r =>
    isWithinRange(r, lastButtonPush, now)
  );
  debug("check: %s", !anyReset);
  return !anyReset;
}

function calmDown(now, lastButtonPush) {
  return differenceInMinutes(now, lastButtonPush) > 5;
}

function flatten(xs) {
  return xs.reduce((a, b) => a.concat(b), []);
}

module.exports = { check, calmDown };
