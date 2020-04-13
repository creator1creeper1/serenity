load("test-common.js");

try {
  assert(typeof this === "object");
  assert(this === globalThis);

  function Foo() {
    this.x = 5;
    assert(typeof this === "object");
    assert(this.x === 5);
  }

  new Foo();
  console.log("PASS");
} catch (err) {
  console.log("FAIL: " + err);
}
