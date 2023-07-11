package edu.iastate.cs228.hw1;

/**
 * This class sorts all the points in an array of 2D points to determine a reference point whose x and y 
 * coordinates are respectively the medians of the x and y coordinates of the original points. 
 * It records the employed sorting algorithm as well as the sorting time for comparison. 
 *
 * @author - Tyler Evans
 */

import java.io.File;
import java.io.FileNotFoundException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.InputMismatchException;
import java.util.Scanner;

public class PointScanner {
	private Point[] points; 
	
	private Point medianCoordinatePoint;  // point whose x and y coordinates are respectively the medians of 
	                                      // the x coordinates and y coordinates of those points in the array points[].
	private Algorithm sortingAlgorithm;    
	
	protected long scanTime; 	       // execution time in nanoseconds. 
	
	protected String outputFileName;
	
	/**
	 * This constructor accepts an array of points and one of the four sorting algorithms as input. Copy 
	 * the points into the array points[].
	 * 
	 * @param  pts - input array of points 
	 * @throws IllegalArgumentException if pts == null or pts.length == 0.
	 */
	public PointScanner(Point[] pts, Algorithm algo) throws IllegalArgumentException {
		if (pts == null) {
			throw new IllegalArgumentException("PointScanner argument pts was null.");
		} else if (pts.length == 0) {
			throw new IllegalArgumentException("PointScanner argument pts has length 0.");
		}
		
		points = new Point[pts.length];
		for (int i = 0; i < pts.length; i++) {
			points[i] = new Point(pts[i]);
		}
		
		this.sortingAlgorithm = algo;
	}
	
	/**
	 * This constructor reads points from a file. 
	 * 
	 * @param  inputFileName - the String representation of the input file location
	 * @throws FileNotFoundException 
	 * @throws InputMismatchException if the input file contains an odd number of integers
	 */
	protected PointScanner(String inputFileName, Algorithm algo) throws FileNotFoundException, InputMismatchException {
		// load the file
		File inputFile = new File(inputFileName);
		Scanner scan = new Scanner(inputFile);
		
		// create temp storage container to hold ints from file
		ArrayList<Integer> tmp = new ArrayList<Integer>();
		
		// load ints from file
		while (scan.hasNextInt()) {
			tmp.add(scan.nextInt());
		}
		
		scan.close();
		
		// make sure there is an even number of ints scanned from file
		if ((tmp.size() % 2) != 0) {
			throw new InputMismatchException("Input file contains an odd number of values.");
		}
		
		// create the points and store them in the array
		points = new Point[tmp.size() / 2];
		int ptIndex = 0;
		for (int i = 0; i < tmp.size(); i+= 2) {
			Point p = new Point(tmp.get(i), tmp.get(i + 1));
			points[ptIndex] = p;
			ptIndex++;
		}
		
		sortingAlgorithm = algo;
	}
	
	/**
	 * Carry out two rounds of sorting using the algorithm designated by sortingAlgorithm as follows:  
	 *    
	 *     a) Sort points[] by the x-coordinate to get the median x-coordinate. 
	 *     b) Sort points[] again by the y-coordinate to get the median y-coordinate.
	 *     c) Construct medianCoordinatePoint using the obtained median x- and y-coordinates.     
	 *  
	 * Based on the value of sortingAlgorithm, create an object of SelectionSorter, InsertionSorter, MergeSorter,
	 * or QuickSorter to carry out sorting.
	 */
	public void scan() { 
		AbstractSorter aSorter;
		
		// create an object for aSorter to reference
		if (sortingAlgorithm == Algorithm.SelectionSort) {
			aSorter = new SelectionSorter(this.points);
		} else if (sortingAlgorithm == Algorithm.InsertionSort) {
			aSorter = new InsertionSorter(this.points);
		} else if (sortingAlgorithm == Algorithm.MergeSort) {
			aSorter = new MergeSorter(this.points);
		} else {
			aSorter = new QuickSorter(this.points);
		}
		
		// for each of the two rounds of sorting, have aSorter do the following:
		//
		//	   a) call setComparator() with an argument 0 or 1.
		//
		//     b) call sort().
		//
		//     c) use a new Point object to store the coordinates of the Median Coordinate Point
		//
		//     d) set the medianCoordinatePoint reference to the object with the correct coordinates.
		//
		//     e) sum up the times spent on the two sorting rounds and set the instance variable scanTime.
		
		int x = 0;
		int y = 0;
		
		// get the start time
		long startTime = System.nanoTime();
		
		for (int i = 0; i < 2; i++) {
			// first set the comparator
			aSorter.setComparator(i);
			
			// start the sort
			aSorter.sort();
			
			// get the median values to create the medianCoordinatePoint
			if (i == 0) {
				x = aSorter.getMedian().getX();
			} else {
				y = aSorter.getMedian().getY();
			}
			
			medianCoordinatePoint = new Point(x, y);
		}
		// System.out.println(aSorter.algorithm + " MCP: " + medianCoordinatePoint.toString());
		
		long endTime = System.nanoTime();
		
		// assign the total sort time
		scanTime = endTime - startTime;
	}
	
	/**
	 * Outputs performance statistics in the format: 
	 * 
	 * <sorting algorithm> <size>  <time>
	 * 
	 * For instance, 
	 * 
	 * selection sort   1000	  9200867
	 * 
	 * Use the spacing in the sample run in Section 2 of the project description. 
	 */
	public String stats() {
		return String.format("%-17s %-10d %-10d", sortingAlgorithm, points.length, scanTime);
	}
	
	/**
	 * Write MCP after a call to scan(),  in the format "MCP: (x, y)"   The x and y coordinates of the point are displayed on the same line with exactly one blank space 
	 * in between. 
	 */
	@Override
	public String toString() {
		return "MCP: (" + medianCoordinatePoint.getX() + ", " + medianCoordinatePoint.getY() + ")";
	}

	/**
	 *  
	 * This method, called after scanning, writes point data into a file by outputFileName. The format 
	 * of data in the file is the same as printed out from toString().  The file can help you verify 
	 * the full correctness of a sorting result and debug the underlying algorithm. 
	 * 
	 * @throws FileNotFoundException
	 */
	public void writeMCPToFile() throws FileNotFoundException {
		try {
			PrintWriter out = new PrintWriter(outputFileName);
			out.println(this.toString());
			out.close();
		} catch (FileNotFoundException e) {
			throw e;
		}
	}	
}
