package edu.iastate.cs228.hw1;

/**
 * This class implements the version of the quicksort algorithm presented in the lecture.   
 *
 * @author - Tyler Evans
 */

public class QuickSorter extends AbstractSorter {
		
	/** 
	 * Constructor takes an array of points.  It invokes the superclass constructor, and also 
	 * sets the instance variable algorithm in the superclass.
	 *   
	 * @param pts - the array of Point objects to sort
	 */
	public QuickSorter(Point[] pts) {
		super(pts);
		algorithm = "quicksort";
	}
		
	/**
	 * Carry out quicksort on the array points[] of the AbstractSorter class.  
	 */
	@Override 
	public void sort() {
		quickSortRec(0, points.length - 1);
	}
	
	/**
	 * Operates on the subarray of points[] with indices between first and last. 
	 * 
	 * @param first - starting index of the subarray
	 * @param last - ending index of the subarray
	 */
	private void quickSortRec(int first, int last) {
		if (first >= last) {
			return;
		}
		
		int p = partition(first, last);
		
		quickSortRec(first, p - 1);
		quickSortRec(p + 1, last);
	}
	
	/**
	 * Operates on the subarray of points[] with indices between first and last.
	 * 
	 * @param first - starting index of the subarray
	 * @param last - ending index of the subarray
	 * @return - the index of the partition
	 */
	private int partition(int first, int last) {
		// use the last element as the partition
		Point partition = points[last];
		
		int i = first - 1;
		for (int j = first; j < last; j++) {
			// compare each Point to the partition
			if (pointComparator.compare(points[j], partition) <= 0) {
				i++;
				swap(i, j);
			}
		}
		
		swap(i + 1, last);
		return i + 1; 
	}
}
