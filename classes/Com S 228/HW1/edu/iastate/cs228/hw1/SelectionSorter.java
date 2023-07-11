package edu.iastate.cs228.hw1;

/**
 * This class implements selection sort.   
 *
 * @author - Tyler Evans
 */

public class SelectionSorter extends AbstractSorter {
	
	/**
	 * Constructor takes an array of points.  It invokes the superclass constructor, and also 
	 * sets the instance variable algorithm in the superclass.
	 *  
	 * @param pts - the array of Point objects to sort
	 */
	public SelectionSorter(Point[] pts) {
		super(pts);
		algorithm = "selection sort";
	}	

	/** 
	 * Apply selection sort on the array points[] of the parent class AbstractSorter.
	 */
	@Override 
	public void sort() {
		for (int i = 0; i < points.length - 1; i++) {
			int min = i;
			for (int j = i + 1; j < points.length; j++) {
				if (pointComparator.compare(points[j], points[min]) < 0) {
					min = j;
				}
			}
			swap(i, min);
		}
	}	
}
