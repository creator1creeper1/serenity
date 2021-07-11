describe("correct behavior", () => {
    test("basic functionality", () => {
        const instant1 = new Temporal.Instant(111n);
        expect(instant1.equals(instant1));
        const instant2 = new Temporal.Instant(999n);
        expect(!instant1.equals(instant2));
    });
});

test("errors", () => {
    test("this value must be a Temporal.Instant object", () => {
        expect(() => {
            Temporal.Instant.prototype.equals.call("foo", 1, 2);
        }).toThrowWithMessage(TypeError, "Not a Temporal.Instant");
    });
});
