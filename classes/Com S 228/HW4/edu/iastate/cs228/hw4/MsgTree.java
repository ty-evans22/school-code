package edu.iastate.cs228.hw4;

/**
 * Creates a binary tree representation of various characters and their bit codes, and uses it to
 * reconstruct/unzip a message.
 * @author Tyler Evans
 */

import java.io.File;
import java.io.FileNotFoundException;
import java.util.Scanner;

public class MsgTree {
	
	public char payloadChar;
	public MsgTree left;
	public MsgTree right;
	
	/**
	 * Static integer used to keep track of which character in the encoding string the algorithm is currently on.
	 */
	private static int staticCharIdx = 0;
	
	/**
	 * Constructor for building the tree recursively from a string.
	 * @param encodingString - the String representation of the encoding scheme
	 */
	public MsgTree(String encodingString) {
		staticCharIdx = 0;
		MsgTree tree = buildNextTree(encodingString);
		payloadChar = tree.payloadChar;
		left = tree.left;
		right = tree.right;	
	}
	
	/**
	 * Private method that carries out the actual recursive process of building the tree.
	 * @param encodingString - the String representation of the encoding scheme
	 * @return - the tree
	 */
	private MsgTree buildNextTree (String encodingString) {
		char c = encodingString.charAt(staticCharIdx++);
		MsgTree tree = new MsgTree(c);
		if (c == '^') {
			tree.left = buildNextTree(encodingString);
			tree.right = buildNextTree(encodingString);
		}
		return tree;
	}
	
	/**
	 * Constructor for a leaf node representing a character.
	 * @param payloadChar - the character the node represents
	 */
	public MsgTree(char payloadChar) {
		left = null;
		right = null;
		this.payloadChar = payloadChar;
	}
	
	/**
	 * Prints out characters and their binary codes to console.
	 * @param root - the root node of the binary tree
	 * @param code - the code (so far) of the character that the algorithm is working on
	 */
	public static void printCodes(MsgTree root, String code) {
		if (root != null) {
			if (root.payloadChar == '^') {
				printCodes(root.left, code + "0");
				printCodes(root.right, code + "1");
			} else if (root.payloadChar == '\n') {
				System.out.println("   \\n      " + code);
			}
			else {
				System.out.println("   " + root.payloadChar + "       " + code);
			}
		}
	}
	
	/**
	 * Decodes the message from a bit string.
	 * @param codes - the binary tree
	 * @param msg - the bit string representation of the message
	 */
	public void decode(MsgTree codes, String msg) {
		MsgTree root = codes;
		String decodedMsg = "";
		int currBitIdx = 0;
		while (currBitIdx < msg.length()) {
			while (codes.left != null && codes.right != null) {
				if (currBitIdx >= msg.length())
					break;
				char bit = msg.charAt(currBitIdx);
				if (bit == '0')
					codes = codes.left;
				else
					codes = codes.right;
				currBitIdx++;
			}
			decodedMsg += codes.payloadChar;
			codes = root;
		}
		System.out.println(decodedMsg);
	}
	
	public static void main(String[] args) {
		System.out.print("Please enter filename to decode: ");
		Scanner s = new Scanner(System.in);
		String fileName = s.nextLine();
		s.close();
		System.out.println();
		try {
			File file = new File(fileName);
			Scanner scnr = new Scanner(file);
			String encodingScheme = "";
			String bitString = null;
			int currLine = 1;
			while (scnr.hasNextLine()) {
				String tmp = scnr.nextLine();
				if (scnr.hasNextLine()) {
					if (currLine == 2)
						encodingScheme += "\n";
					encodingScheme += tmp;
				}
				else
					bitString = tmp;
				currLine++;
			}
			MsgTree root = new MsgTree(encodingScheme);
			System.out.println("character  code\n-------------------------");
			printCodes(root, "");
			System.out.println("\nMESSAGE:");
			root.decode(root, bitString);
			scnr.close();
		} catch (FileNotFoundException e) {
			System.out.println("File not found.");
			e.printStackTrace();
		}
	}
}
