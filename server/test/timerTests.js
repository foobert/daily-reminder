const { expect } = require("chai");
const { check } = require("../lib/timer");

describe("timer", () => {
  const resetTimes = [6 * 60, 18 * 60];

  it("should be true after a button push", () => {
    const now = new Date("2000-01-01 10:00");
    const buttonPush = new Date("2000-01-01 09:00");

    const res = check(now, buttonPush, resetTimes);

    expect(res).to.be.true;
  });

  it("should be false after reset", () => {
    const now = new Date("2000-01-01 19:00");
    const buttonPush = new Date("2000-01-01 09:00");

    const res = check(now, buttonPush, resetTimes);

    expect(res).to.be.false;
  });

  it("should be false after reset on the next morning", () => {
    const now = new Date("2000-01-02 01:00");
    const buttonPush = new Date("2000-01-01 09:00");

    const res = check(now, buttonPush, resetTimes);

    expect(res).to.be.false;
  });

  it("should be true after push on the next morning", () => {
    const now = new Date("2000-01-02 01:00");
    const buttonPush = new Date("2000-01-01 19:00");

    const res = check(now, buttonPush, resetTimes);

    expect(res).to.be.true;
  });

  it("should be false when there is no button push", () => {
    expect(check(new Date(), null, resetTimes)).to.be.false;
  });
});
