package edu.iastate.cs228.hw1;

/**
 * This class implements the mergesort algorithm.   
 *
 * @author - Tyler Evans
 */

public class MergeSorter extends AbstractSorter {
	
	/** 
	 * Constructor takes an array of points.  It invokes the superclass constructor, and also 
	 * sets the instance variable algorithm in the superclass.
	 *  
	 * @param pts - the array of Point objects to sort
	 */
	public MergeSorter(Point[] pts) {
		super(pts);
		algorithm = "mergesort";
	}

	/**
	 * Perform mergesort on the array points[] of the parent class AbstractSorter.
	 */
	@Override 
	public void sort() {
		mergeSortRec(points); 
	}

	/**
	 * This is a recursive method that carries out mergesort on an array pts[] of points. One 
	 * way is to make copies of the two halves of pts[], recursively call mergeSort on them, 
	 * and merge the two sorted subarrays into pts[].   
	 * 
	 * @param pts - point array 
	 */
	private void mergeSortRec(Point[] pts) {
		// get the length of the array
		int a = pts.length;
		int b = a / 2;
		
		if (a <= 1) {
			return;
		}
		
		// create new arrays for left and right
		Point[] left = new Point[b];
		Point[] right = new Point[a - b];
		
		// fill the left array
		for (int i = 0; i < b; i++) {
			left[i] = pts[i];
		}
		
		// fill the right array
		int c = 0;
		for (int i = b; i < a; i++) {
			right[c] = pts[i];
			c++;
		}
		
		// call mergeSortRec() for both new arrays
		mergeSortRec(left);
		mergeSortRec(right);
		
		// merge left and right
		Point[] tmp = new Point[pts.length];
		tmp = merge(left, right);
		
		for (int i = 0; i < tmp.length; i++) {
			pts[i] = new Point(tmp[i]);
		}
	}
	
	/**
	 * Merges two Point arrays.
	 * @param left - the first array to merge
	 * @param right - the second array to merge
	 * @return - the merged array
	 */
	private Point[] merge(Point[] left, Point[] right) {
		int leftLength = left.length;
		int rightLength = right.length;
		
		Point[] result = new Point[leftLength + rightLength];
		
		int leftIndex = 0;
		int rightIndex = 0;
		int i = 0;
		while ((leftIndex < leftLength) && (rightIndex < rightLength)) {
			if (pointComparator.compare(left[leftIndex], right[rightIndex]) <= 0) {
				result[i++] = left[leftIndex];
				leftIndex++;
			} else {
				result[i++] = right[rightIndex];
				rightIndex++;
			}
		}
		
		if (leftIndex >= leftLength) {
			for (int j = rightIndex; j < rightLength; j++) {
				result[i++] = right[j];
			}
		} else {
			for (int j = leftIndex; j < leftLength; j++) {
				result[i++] = left[j];
			}
		}
		
		return result;
	}
}
