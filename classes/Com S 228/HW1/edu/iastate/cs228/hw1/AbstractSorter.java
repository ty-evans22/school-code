package edu.iastate.cs228.hw1;

/**
 * 
 * This abstract class is extended by SelectionSort, InsertionSort, MergeSort, and QuickSort.
 * It stores the input (later the sorted) sequence. 
 *
 * @author - Tyler Evans
 */

import java.lang.IllegalArgumentException;
import java.util.Comparator;

public abstract class AbstractSorter {
	
	protected Point[] points;    // array of points operated on by a sorting algorithm. 
	                             // stores ordered points after a call to sort(). 
	
	protected String algorithm = null; // "selection sort", "insertion sort", "mergesort", or
	                                   // "quicksort". Initialized by a subclass constructor.
		 
	protected Comparator<Point> pointComparator = null;  
	
	/**
	 * This constructor accepts an array of points as input. Copy the points into the array points[]. 
	 * 
	 * @param  pts  input array of points 
	 * @throws IllegalArgumentException if pts == null or pts.length == 0.
	 */
	protected AbstractSorter(Point[] pts) throws IllegalArgumentException {
		if (pts == null) {
			throw new IllegalArgumentException("AbstractSorter argument pts was null.");
		} else if (pts.length == 0) {
			throw new IllegalArgumentException("AbstractSorter argument pts had length 0.");
		}
		
		getPoints(pts);
	}

	/**
	 * Generates a comparator on the fly that compares by x-coordinate if order == 0, by y-coordinate
	 * if order == 1. Assign the 
     * comparator to the variable pointComparator.  
	 * 
	 * @param order  0   by x-coordinate 
	 * 				 1   by y-coordinate
	 * 			    
	 * @throws IllegalArgumentException if order is less than 0 or greater than 1     
	 */
	public void setComparator(int order) throws IllegalArgumentException {
		if (order == 0 || order == 1) {
			if (order == 0) {
				// compare by x-coordinate
				Point.setXorY(true);
			} else {
				// compare by y-coordinate
				Point.setXorY(false);
			}
			
			pointComparator = new Comparator<Point>() {
				@Override
				public int compare(Point p1, Point p2) {
					return p1.compareTo(p2);
				}
			};
		} else {
			throw new IllegalArgumentException("Parameter order in AbstractSorter.setComparator is outside the interval [0,1].");
		}
	}

	/**
	 * Use the created pointComparator to conduct sorting.  
	 * 
	 * Ought to be protected. Made public for testing. 
	 */
	public abstract void sort();
	
	/**
	 * Obtain the point in the array points[] that has median index 
	 * 
	 * @return	median point 
	 */
	public Point getMedian() {
		return points[points.length/2]; 
	}
	
	/**
	 * Copys the array points[] onto the array pts[]. 
	 * 
	 * @param pts
	 */
	public void getPoints(Point[] pts) {
		points = new Point[pts.length];
		for (int i = 0; i < pts.length; i++) {
			points[i] = new Point(pts[i]);
		} 
	}

	/**
	 * Swaps the two elements indexed at i and j respectively in the array points[]. 
	 * 
	 * @param i - the original Point index
	 * @param j - the index to swap the Point to
	 */
	protected void swap(int i, int j) {
		// need to check and make sure i and j are in range
		Point tmp = new Point(points[i]);
		points[i] = new Point(points[j]);
		points[j] = new Point(tmp);
	}	
}
