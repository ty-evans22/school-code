package edu.iastate.cs228.hw1;

import java.util.Random;

public class Test {
	public static void main(String[] args) {
		Random generator = new Random();
		int numPoints = 1000;
		
		Point[] p = new Point[numPoints];
		
		for (int i = 0; i < numPoints; i++) {
			int x = generator.nextInt(101) - 50;
			int y = generator.nextInt(101) - 50;
			p[i] = new Point(x, y);
			
			System.out.println("Point " + i + ": " + p[i].toString());
		}
		System.out.println();
		
		PointScanner qs = new PointScanner(p, Algorithm.QuickSort);
		qs.scan();
		System.out.println(qs.stats());
		
		PointScanner ms = new PointScanner(p, Algorithm.MergeSort);
		ms.scan();
		System.out.println(ms.stats());
		
		PointScanner is = new PointScanner(p, Algorithm.InsertionSort);
		is.scan();
		System.out.println(is.stats());
		
		PointScanner ss = new PointScanner(p, Algorithm.SelectionSort);
		ss.scan();
		System.out.println(ss.stats());
	}
}
