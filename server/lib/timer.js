const {
  addMinutes,
  startOfDay,
  isSameDay,
  isWithinRange,
  subDays,
  differenceInMinutes
} = require("date-fns");

function check(now, lastButtonPush, resetTimes) {
  if (!lastButtonPush) {
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
  return !anyReset;
}

function calmDown(now, lastButtonPush) {
  return differenceInMinutes(now, lastButtonPush) > 5;
}

function flatten(xs) {
  return xs.reduce((a, b) => a.concat(b), []);
}

module.exports = { check, calmDown };
