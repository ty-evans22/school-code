package edu.iastate.cs228.hw1;

/**
 * This class executes four sorting algorithms: selection sort, insertion sort, mergesort, and
 * quicksort, over randomly generated integers as well integers from a file input. It compares the 
 * execution times of these algorithms on the same input. 
 *
 * @author - Tyler Evans
 */

import java.io.FileNotFoundException;
import java.util.Scanner; 
import java.util.Random; 


public class CompareSorters 
{
	/**
	 * Repeatedly take integer sequences either randomly generated or read from files. 
	 * Use them as coordinates to construct points.  Scan these points with respect to their 
	 * median coordinate point four times, each time using a different sorting algorithm.  
	 * 
	 * @param args
	 **/
	public static void main(String[] args) throws FileNotFoundException {		
		// Conducts multiple rounds of comparison of four sorting algorithms.  Within each round, 
		// set up scanning as follows: 
		// 
		//    a) If asked to scan random points, calls generateRandomPoints() to initialize an array 
		//       of random points. 
		// 
		//    b) Reassigns to the array scanners[] (declared below) the references to four new 
		//       PointScanner objects, which are created using four different values  
		//       of the Algorithm type:  SelectionSort, InsertionSort, MergeSort and QuickSort. 
		
		System.out.println("Performance of Four Sorting Algorithms in Point Scanning");
		System.out.println();
		System.out.println("keys 1 (random integers) 2 (file input) 3 (exit)");
		
		Scanner scan = new Scanner(System.in);
		int selection = 1;
		int trial = 1;
		
		PointScanner[] scanners = new PointScanner[4];
		
		Random generator = new Random();
		
		while(true) {
			System.out.print("Trial " + trial + ": ");
			selection = scan.nextInt();
			
			if (selection != 1 && selection != 2) {
				scan.close();
				System.exit(0);
			} else {
				if (selection == 1) {
					// random points
					System.out.print("Enter number of random points: ");
					int numPoints = scan.nextInt();
					
					Point[] p = generateRandomPoints(numPoints, generator);
					
					// initialize array with generated points
					scanners[0] = new PointScanner(p, Algorithm.SelectionSort);
					scanners[1] = new PointScanner(p, Algorithm.InsertionSort);
					scanners[2] = new PointScanner(p, Algorithm.MergeSort);
					scanners[3] = new PointScanner(p, Algorithm.QuickSort);
				} else {
					// points from file
					System.out.println("Points from a file.");
					System.out.print("File name: ");
					String fileName = scan.next();
					
					// initialize array using file
					scanners[0] = new PointScanner(fileName, Algorithm.SelectionSort);
					scanners[1] = new PointScanner(fileName, Algorithm.InsertionSort);
					scanners[2] = new PointScanner(fileName, Algorithm.MergeSort);
					scanners[3] = new PointScanner(fileName, Algorithm.QuickSort);
				}
			}
			
			// For each input of points, do the following. 
			// 
			//     a) Initialize the array scanners[].  
			//
			//     b) Iterate through the array scanners[], and have every scanner call the scan() 
			//        method in the PointScanner class.  
			//
			//     c) After all four scans are done for the input, print out the statistics table from
			//		  section 2.
			
			for (int i = 0; i < scanners.length; i++) {
				scanners[i].scan();
			}
			
			// print summary
			System.out.println();
			System.out.printf("%-17s %-10s %-10s \n", "algorithm", "size", "time (ns)");
			System.out.println("--------------------------------------");
			
			for (int i = 0; i < scanners.length; i++) {
				System.out.println(scanners[i].stats());
			}
			
			System.out.println("--------------------------------------");
			System.out.println();
			
			trial++;
		}
	}
	
	/**
	 * This method generates a given number of random points.
	 * The coordinates of these points are pseudo-random numbers within the range 
	 * [-50,50] � [-50,50]. Please refer to Section 3 on how such points can be generated.
	 * 
	 * Ought to be private. Made public for testing. 
	 * 
	 * @param numPts - number of points
	 * @param rand - Random object to allow seeding of the random number generator
	 * @throws IllegalArgumentException if numPts < 1
	 */
	public static Point[] generateRandomPoints(int numPts, Random rand) throws IllegalArgumentException {
		if (numPts < 1) {
			throw new IllegalArgumentException("Parameter numPts in CompareSorters.generateRandomPoints has length less than 1.");
		}
		
		Point[] p = new Point[numPts];
		for (int i = 0; i < numPts; i++) {
			int x = rand.nextInt(101) - 50;
			int y = rand.nextInt(101) - 50;
			p[i] = new Point(x, y);
		}
		return p; 
	}
}
