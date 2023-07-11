package edu.iastate.cs228.hw3;

import java.util.Stack;

/**
 * Class containing representation of an infix expression, and methods to convert it to a postfix expression.
 * @author - Tyler Evans
 */

public class InfixExpression {
	
	/**
	 * The stack containing all of the tokens within the infix expression.
	 */
	public Stack<String> stack;
	public Stack<String> opStack;
	
	/**
	 * Creates a new InfixExpression object from a given string. Each token within the expression
	 * is pushed into the stack.
	 * @param s - the string representation of the infix expression
	 */
	public InfixExpression(String s) {
		stack = new Stack<String>();
		opStack = new Stack<String>();
		String[] tokens = s.split(" ");
		for (int i = tokens.length - 1; i >= 0; i--) {
			stack.push(tokens[i]);
		}
	}
	
	/**
	 * Checks whether or not a given token is an operator.
	 * @param token - the token to be checked
	 * @return
	 * 		true if token is an operator, false otherwise
	 */
	private boolean isOp(String token) {
		String[] ops = {"+", "-", "*", "/", "%", "^", "(", ")"};
		for (int i = 0; i < 8; i++) {
			if (token.equals(ops[i]))
				return true;
		}
		return false;
	}
	
	/**
	 * Gets the stack precedence of a given operator.
	 * @param op - the operator to be checked
	 * @return
	 * 		the stack precedence of the operator
	 */
	private int stackPrec(String op) {
		if (op.equals("+") || op.equals("-"))
			return 1;
		else if (op.equals("*") || op.equals("/") || op.equals("%"))
			return 2;
		else if (op.equals("^"))
			return 3;
		else if (op.equals("("))
			return -1;
		return 0;
	}
	
	/**
	 * Gets the input precedence of a given operator.
	 * @param op - the operator to be checked
	 * @return
	 * 		the input precedence of the given operator
	 */
	private int inputPrec(String op) {
		if (op.equals("+") || op.equals("-"))
			return 1;
		else if (op.equals("*") || op.equals("/") || op.equals("%"))
			return 2;
		else if (op.equals("^"))
			return 4;
		else if (op.equals("("))
			return 5;
		return 0;
	}
	
	/**
	 * Gets the rank of a given token.
	 * @param token - the token to be checked
	 * @return
	 * 		the rank of the given token
	 */
	private int rank(String token) {
		if (token.equals("+") || token.equals("-") || token.equals("*") || token.equals("/") ||
				token.equals("%") || token.equals("^"))
			return -1;
		else if (token.equals("(") || token.equals(")"))
			return 0;
		return 1;
	}
	
	/**
	 * Compares operators and returns the string of operators to be appended to the output. If there is a missing
	 * '(' or a free floating pair of parenthesis, an error string will be returned instead.
	 * @param op - the current operator
	 * @return
	 * 		the string to be appended to the output
	 */
	private String outputHigherOrEqual(String op) {
		String output = "";
		int popped = 0;
		while(!opStack.isEmpty() && inputPrec(op) <= stackPrec(opStack.peek())) {
			String o = opStack.pop();
			popped++;
			output += " " + o;
		}
		if (op.equals(")")) {
			if (opStack.isEmpty())
				output = "msng ( error";
			else if (popped == 0)
				output = "free fltg error";
			else
				opStack.pop();
		}
		else
			opStack.push(op);
		return output;
	}
	
	/**
	 * Gets the string representation of the postfix conversion of this infix expression. If there is an error, that
	 * will be output instead.
	 * @return
	 * 		the postfix conversion of the infix expression
	 */
	public String postfix() {
		String output = "";
		boolean error = false;
		int cumRank = 0;
		
		while (!stack.isEmpty() && !error) {
			String token = stack.pop();
			cumRank += rank(token);
			
			if (cumRank > 1) {
				output = "Error: too many operands (" + token + ")";
				error = true;
				break;
			} else if (cumRank < 0) {
				output = "Error: too many operators (" + token + ")";
				error = true;
				break;
			}
			
			if (isOp(token)) {
				String append = outputHigherOrEqual(token);
				if (append.equals("msng ( error")) {
					output = "Error: no opening parenthesis detected";
					error = true;
					break;
				} else if (append.equals("free fltg error")) {
					output = "Error: no subexpression detected ()";
					error = true;
					break;
				}
				output += append;
			} else {
				output += " " + token;
			}
		}
		
		if (!error) {
			while(!opStack.isEmpty()) {
				String token = opStack.pop();
				if (token.equals("(")) {
					output = "Error: no closing parenthesis detected";
					break;
				}
				output += " " + token;
			}
			output = output.trim();
		}
		
		return output;
	}
}
