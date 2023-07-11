package edu.iastate.cs228.hw3;

public class Test {

	public static void main(String[] args) {
//		testInfixCreation();
//		testStackPrec();
//		testInputPrec();
		testPostfix();
	}

	/**
	 * Tests the creation of an InfixExpression.
	 */
	private static void testInfixCreation() {
		// the various strings to test
		String s = "5 * 34 + -2 - 1";
		
		// creates the InfixExpression objects
		InfixExpression ie = new InfixExpression(s);
		
		// prints out the InfixExpression stacks to the console
		System.out.println(ie.stack.toString());
	}
	
	/**
	 * Tests stack precedence of the operators.
	 * NOTE: Need to change stackPrec() method from private to public in order to test.
	 */
//	private static void testStackPrec() {
//		InfixExpression ie = new InfixExpression("");
//		String[] ops = {"+", "-", "*", "/", "%", "^", "(", ")"};
//		
//		for (int i = 0; i < ops.length; i++) {
//			System.out.println(ops[i] + ": " + ie.stackPrec(ops[i]));
//		}
//	}
	
	/**
	 * Tests input precedence of the operators.
	 * NOTE: Need to change inputPrec() method from private to public in order to test.
	 */
//	private static void testInputPrec() {
//		InfixExpression ie = new InfixExpression("");
//		String[] ops = {"+", "-", "*", "/", "%", "^", "(", ")"};
//		
//		for (int i = 0; i < ops.length; i++) {
//			System.out.println(ops[i] + ": " + ie.inputPrec(ops[i]));
//		}
//	}
	
	private static void testPostfix() {
		String infix = "72 + 8 ^ 3 ^ 4";
		InfixExpression ie = new InfixExpression(infix);
		System.out.println(ie.postfix());
	}
}
