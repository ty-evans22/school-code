package edu.iastate.cs228.hw1;

/**
 * This class implements insertion sort.   
 *
 * @author - Tyler Evans
 */

public class InsertionSorter extends AbstractSorter { 
	
	/**
	 * Constructor takes an array of points.  It invokes the superclass constructor, and also 
	 * sets the instance variable algorithm in the superclass.
	 * 
	 * @param pts - the array of Point objects to sort
	 */
	public InsertionSorter(Point[] pts) {
		super(pts);
		algorithm = "insertion sort";
	}	

	/** 
	 * Perform insertion sort on the array points[] of the parent class AbstractSorter.  
	 */
	@Override 
	public void sort() {
		for (int i = 1; i < points.length; i++) {
			Point tmp = points[i];
			int j = i - 1;
			while((j > -1) && (pointComparator.compare(points[j], tmp) > 0)) {
				points[j + 1] = points[j];
				j--;
			}
			points[j + 1] = tmp;
		}
	}		
}
