package edu.iastate.cs228.hw2;

public class Test {

	public static void main(String[] args) {
		// System.out.println(PrimeFactorization.isPrime(29));
		
		//testListTraversal();
		
		PrimeFactorization pf1 = new PrimeFactorization(12);
		PrimeFactorization pf2 = new PrimeFactorization(42);
//		pf1.dividedBy(pf2);
//		System.out.println(pf1);
//		System.out.println(pf1.value());
//		System.out.println();
		
		System.out.println(pf1.lcm(pf2));
	}
}
