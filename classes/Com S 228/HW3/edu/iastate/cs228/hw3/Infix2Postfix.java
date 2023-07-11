package edu.iastate.cs228.hw3;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Scanner;

/**
 * Takes an input file containing line(s) of infix expressions and converts them into postfix expressions, then outputs
 * the postfix expressions to an output file.
 * @author - Tyler Evans
 */

public class Infix2Postfix {
	
	public static void main(String[] args) {
		try {
			File input = new File("input.txt");
			Scanner scnr = new Scanner(input);
			FileWriter fw = new FileWriter("output.txt");
			while (scnr.hasNextLine()) {
				InfixExpression ie = new InfixExpression(scnr.nextLine());
				String pf = ie.postfix();
				fw.write(pf);
				if (scnr.hasNextLine())
					fw.write("\n");
			}
			scnr.close();
			fw.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
