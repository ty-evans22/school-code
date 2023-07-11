package edu.iastate.cs228.hw2;

/**
 * 
 * @author - Tyler Evans
 */

import java.util.ListIterator;

public class PrimeFactorization implements Iterable<PrimeFactor> {

	private static final long OVERFLOW = -1;
	private long value; // the factored integer
						// it is set to OVERFLOW when the number is greater than 2^63-1, the
						// largest number representable by the type long.

	/**
	 * Reference to dummy node at the head.
	 */
	private Node head;

	/**
	 * Reference to dummy node at the tail.
	 */
	private Node tail;

	private int size; // number of distinct prime factors

	/**
	 * Default constructor constructs an empty list to represent the number 1.
	 * 
	 * Combined with the add() method, it can be used to create a prime
	 * factorization.
	 */
	public PrimeFactorization() {
		head = new Node();
		tail = new Node();
		head.next = tail;
		tail.previous = head;
		value = 1;
	}

	/**
	 * Obtains the prime factorization of n and creates a doubly linked list to
	 * store the result.
	 * Follows the direct search factorization algorithm in Section 1.2 of the project description.
	 * 
	 * @param n
	 * @throws IllegalArgumentException if n < 1
	 */
	public PrimeFactorization(long n) throws IllegalArgumentException {
		this();
		if (n < 1)
			throw new IllegalArgumentException("Value of n in constructor PrimeFactorization(long n) cannot be less than 1.");
		if (n == 2 || n == 3)
			add((int) n, 1);
		else {
			long m = n;
			int floor = (int) Math.floor(Math.sqrt(m));
			for (int d = 2; d <= floor; d++) {
				while (m % d == 0) {
					if (isPrime(d))
						add(d, 1);
					m /= d;
				}
			}
			if (m != 1 && isPrime(m))
				add((int) m, 1);
		}
		updateValue();
	}
	
	/**
	 * Copy constructor. It is unnecessary to verify the primality of the numbers in the list.
	 * 
	 * @param pf
	 */
	public PrimeFactorization(PrimeFactorization pf) {
		this();
		multiply(pf);
	}
	
	/**
	 * Constructs a factorization from an array of prime factors. Useful when the number is
	 * too large to be represented even as a long integer.
	 * 
	 * @param pfList
	 */
	public PrimeFactorization(PrimeFactor[] pfList) {
		this();
		for (int i = 0; i < pfList.length; i++) {
			add(pfList[i].prime, pfList[i].multiplicity);
		}
	}
	
	/**
	 * Test if a number is prime or not. Check iteratively from 2 and the largest
	 * integer not exceeding the square root of n to see if it divides n.
	 * 
	 * @param n
	 * @return true if n is prime
	 * 		   false otherwise
	 */
	public static boolean isPrime(long n) {
		if (n < 2)
			return false;
		int floor = (int) Math.floor(Math.sqrt(n));
		for (int i = 2; i <= floor; i++) {
			if (n % i == 0)
				return false;
		}
		return true;
	}
	
	/**
	 * Multiplies the integer v represented by this object with another number n. Note that v may
	 * be too large (in which case this.value == OVERFLOW). You can do this in one loop: Factor n and
	 * traverse the doubly linked list simultaneously. For details refer to Section 3.1 in the
	 * project description. Store the prime factorization of the product. Update value and size.
	 * 
	 * @param n
	 * @throws IllegalArgumentException if n < 1
	 */
	public void multiply(long n) throws IllegalArgumentException {
		if (n < 1)
			throw new IllegalArgumentException("Value of n in PrimeFactorization.multiply(long n) cannot be less than 1.");
		if (n == 2 || n == 3)
			add((int) n, 1);
		else {
			long m = n;
			int floor = (int) Math.floor(Math.sqrt(m));
			for (int d = 2; d <= floor; d++) {
				while (m % d == 0) {
					if (isPrime(d))
						add(d, 1);
					m /= d;
				}
			}
			if (m != 1 && isPrime(m))
				add((int) m, 1);
		}
		updateValue();
	}
	
	/**
	 * Multiplies the represented integer v with another number in the factorization form. Traverse both
	 * linked lists and store the result in this list object. See Section 3.1 in the project description
	 * for details of algorithm.
	 * 
	 * @param pf
	 */
	public void multiply(PrimeFactorization pf) {
		PrimeFactorizationIterator pf1 = this.iterator();
		PrimeFactorizationIterator pf2 = pf.iterator();
		while (pf2.hasNext()) {
			PrimeFactor pFactor = pf2.next();
			while (pf1.hasNext()) {
				if (pf1.next().prime >= pFactor.prime) {
					pf1.previous();
					break;
				}
			}
			pf1.add(pFactor);
		}
		updateValue();
	}
	
	/**
	 * Multiplies the integers represented by two PrimeFactorization objects.
	 * 
	 * @param pf1
	 * @param pf2
	 * @return object of PrimeFactorization to represent the product
	 */
	public static PrimeFactorization multiply(PrimeFactorization pf1, PrimeFactorization pf2) {
		PrimeFactorization pf = new PrimeFactorization(pf1);
		pf.multiply(pf2);
		return pf;
	}
	
	/**
	 * Divides the represented integer v by n. Make updates to the list, value, size if divisible.
	 * No update otherwise. Refer to Section 3.2 in the project description for details.
	 * 
	 * @param n
	 * @return true if divisible
	 * 		   false if not divisible
	 * @throws IllegalArgumentException if n <= 0
	 */
	public boolean dividedBy(long n) throws IllegalArgumentException {
		if (n <= 0)
			throw new IllegalArgumentException("Value n in PrimeFactorization.dividedBy(long n) must be greater than 0.");
		if (this.value != -1 && this.value < n)
			return false;
		else if (this.value == n) {
			clearList();
			return true;
		}
		PrimeFactorization pf = new PrimeFactorization(n);
		return dividedBy(pf);
	}
	
	/**
	 * Division where the divisor is represented in the factorization form. Update the linked
	 * list of this object accordingly by removing those nodes housing prime factors that disappear
	 * after the division. No update if this number is not divisible by pf. Algorithm details are
	 * given in Section 3.2.
	 * 
	 * @param pf
	 * @return true if divisible by pf
	 * 		   false otherwise
	 */
	public boolean dividedBy(PrimeFactorization pf) {
		if (this.value != -1 && pf.value != -1 && this.value < pf.value)
			return false;
		else if (this.value != -1 && pf.value == -1)
			return false;
		else if (this.value == pf.value) {
			clearList();
			return true;
		}	
		PrimeFactorization pf1 = new PrimeFactorization(this);
		PrimeFactorizationIterator iterCopy = pf1.iterator();
		PrimeFactorizationIterator iterPf = pf.iterator();
		while (iterPf.hasNext()) {
			while (iterCopy.cursor.pFactor.prime < iterPf.cursor.pFactor.prime &&
					iterCopy.hasNext())
				iterCopy.next();
			if (!iterCopy.hasNext() && iterPf.hasNext() ||
					iterCopy.cursor.pFactor.prime > iterPf.cursor.pFactor.prime ||
					iterCopy.cursor.pFactor.multiplicity < iterPf.cursor.pFactor.multiplicity)
				return false;
			pf1.remove(iterCopy.next().prime, iterPf.next().multiplicity);
		}
		this.head = pf1.head;
		this.tail = pf1.tail;
		updateValue();
		
		// update size
		PrimeFactorizationIterator pfi = iterator();
		size = 0;
		while (pfi.hasNext()) {
			size++;
			pfi.next();
		}
		
		return true;
	}
	
	/**
	 * Divide the integer represented by the object pf1 by that represented by the object pf2.
	 * Return a new object representing the quotient if divisible. Do not make changes to pf1 and
	 * pf2. No update if the first number is not divisible by the second one.
	 * 
	 * @param pf1
	 * @param pf2
	 * @return quotient as a new PrimeFactorization object if divisible
	 * 		   null otherwise
	 */
	public static PrimeFactorization dividedBy(PrimeFactorization pf1, PrimeFactorization pf2) {
		PrimeFactorization pf = new PrimeFactorization(pf1);
		if (pf.dividedBy(pf2))
			return pf;
		return null;
	}
	
	/**
	 * Computes the greatest common divisor (gcd) of the represented integer v and an input integer n.
	 * Returns the result as a PrimeFactor object. Calls the method Euclidian() if
	 * this.value != OVERFLOW.
	 * 
	 * It is more efficient to factorize the gcd than n, which can be much greater.
	 * 
	 * @param n
	 * @return prime factorization of gcd
	 * @throws IllegalArgumentException if n < 1
	 */
	public PrimeFactorization gcd(long n) throws IllegalArgumentException {
		if (n < 1)
			throw new IllegalArgumentException("Value n in PrimeFactorization.gcd(long n) cannot be less than 1.");
		updateValue();
		return new PrimeFactorization(Euclidian(value, n));
	}
	
	/**
	 * Implements the Euclidian algorithm to compute the gcd of two natural numbers m and n.
	 * The algorithm is described in Section 4.1 of the project description.
	 * 
	 * @param m
	 * @param n
	 * @return gcd of m and n
	 * @throws IllegalArgumentException if m < 1 or n < 1
	 */
	public static long Euclidian(long m, long n) throws IllegalArgumentException {
		if (m < 1 || n < 1)
			throw new IllegalArgumentException("Values of m and n in PrimeFactorization.Euclidian(long m, long n) must both be less than 1.");
		while (m % n != 0) {
			long tmp = n;
			n = m % n;
			m = tmp;
		}
		return n;
	}
	
	/**
	 * Computes the gcd of the values represented by this object and pf by traversing the two lists. No
	 * direct computation involving value and pf.value. Refer to Section 4.2 of the project description
	 * on how to proceed.
	 * 
	 * @param pf
	 * @return prime factorization of the gcd
	 */
	public PrimeFactorization gcd(PrimeFactorization pf) {
		PrimeFactorizationIterator pf1 = iterator();
		PrimeFactorizationIterator pf2 = pf.iterator();
		PrimeFactorization g = new PrimeFactorization();
		while (pf1.hasNext() && pf2.cursor.pFactor != null) {
			while (pf1.cursor.pFactor.prime < pf2.cursor.pFactor.prime)
				pf1.next();
			if (pf1.cursor.pFactor.prime > pf2.cursor.pFactor.prime) {
				if (pf2.hasNext()) {
					pf2.next();
					continue;
				}
				else
					break;
			}
			g.add(pf1.cursor.pFactor.prime, Math.min(pf1.next().multiplicity, pf2.cursor.pFactor.multiplicity));
		}
		return g;
	}
	
	/**
	 * 
	 * @param pf1
	 * @param pf2
	 * @return prime factorization of the gcd of two numbers represented by pf1 and pf2
	 */
	public static PrimeFactorization gcd(PrimeFactorization pf1, PrimeFactorization pf2) {
		PrimeFactorization pf = new PrimeFactorization(pf1);
		return pf.gcd(pf2);
	}
	
	/**
	 * Computes the least common multiple (lcm) of the two integers represented by this object
	 * and pf. The list-based algorithm is given in Section 4.3 of the project description.
	 * @param pf
	 * @return factored least common multiple
	 */
	public PrimeFactorization lcm(PrimeFactorization pf) {
		PrimeFactorizationIterator pf1 = iterator();
		PrimeFactorizationIterator pf2 = pf.iterator();
		PrimeFactorization g = new PrimeFactorization();
		while (pf1.hasNext() && pf2.hasNext()) {
			if (pf1.cursor.pFactor.prime == pf2.cursor.pFactor.prime)
				g.add(pf1.cursor.pFactor.prime, Math.max(pf1.next().multiplicity, pf2.next().multiplicity));
			else if (pf1.cursor.pFactor.prime > pf2.cursor.pFactor.prime)
				g.add(pf2.cursor.pFactor.prime, pf2.next().multiplicity);
			else
				g.add(pf1.cursor.pFactor.prime, pf1.next().multiplicity);
		}
		while (pf1.hasNext())
			g.add(pf1.cursor.pFactor.prime, pf1.next().multiplicity);
		while (pf2.hasNext())
			g.add(pf2.cursor.pFactor.prime, pf2.next().multiplicity);
		return g;
	}
	
	/**
	 * Computes the least common multiple of the represented integer v and an integer n. Construct a
	 * PrimeFactors object using n and then call the lcm() method above. Calls the first lcm() method.
	 * @param n
	 * @return factored least common multiple
	 * @throws IllegalArgumentException if n < 1
	 */
	public PrimeFactorization lcm(long n) throws IllegalArgumentException {
		PrimeFactorization pf = new PrimeFactorization(n);
		return lcm(pf);
	}
	
	/**
	 * Computes the least common multiple of the integers represented by pf1 and pf2.
	 * 
	 * @param pf1
	 * @param pf2
	 * @return prime factorization of the lcm of two numbers represented by pf1 and pf2
	 */
	public static PrimeFactorization lcm(PrimeFactorization pf1, PrimeFactorization pf2) {
		PrimeFactorization pf = new PrimeFactorization(pf1);
		return pf.lcm(pf2);
	}
	
	/**
	 * Traverses the list to determine if p is a prime factor.
	 * 
	 * Precondition: p is a prime.
	 * 
	 * @param p
	 * @return true if p is a prime factor of the number v represented by this linked list
	 * 		   false otherwise
	 * @throws IllegalArgumentException if p is not a prime
	 */
	public boolean containsPrimeFactor(int p) throws IllegalArgumentException {
		if (!isPrime(p))
			throw new IllegalArgumentException("Value p in PrimeFactorization.containsPrimeFactor(int p) is not prime.");
		PrimeFactorizationIterator pfi = iterator();
		while (pfi.hasNext()) {
			if (pfi.next().prime == p)
				return true;
		}
		return false;
	}
	
	/**
	 * Adds a prime factor p of multiplicity m. Search for p in the linked list. If p is found at
	 * a node N, add m to N.multiplicity. Otherwise, create a new node to store p and m.
	 * 
	 * Precondition: p is a prime.
	 * 
	 * @param p - prime
	 * @param m - multiplicity
	 * @return true if m >= 1
	 * 		   false if m < 1
	 */
	public boolean add(int p, int m) {
		PrimeFactorizationIterator pfi = iterator();
		while (pfi.hasNext()) {
			if (pfi.next().prime >= p) {
				pfi.previous();
				break;
			}
		}
		pfi.add(new PrimeFactor(p, m));
		
		if (m >= 1)
			return true;
		return false;
	}
	
	/**
	 * Removes m from the multiplicity of a prime p on the linked list. It starts by searching
	 * for p. Returns false if p is not found, and true if p is found. In the latter case, let
	 * N be the node that stores p. If N.multiplicity > m, subtracts m from N.multiplicity.
	 * If N.multiplicity <= m, removes the node N.
	 * 
	 * Precondition: p is a prime.
	 * 
	 * @param p
	 * @param m
	 * @return true when p is found
	 * 		   false when p is not found
	 * @throws IllegalArgumentException if m < 1
	 */
	public boolean remove(int p, int m) throws IllegalArgumentException {
		if (m < 1)
			throw new IllegalArgumentException("Value m in PrimeFactorization.remove(int p, int m) cannot be less than 1.");
		if (!containsPrimeFactor(p))
			return false;
		PrimeFactorizationIterator pfi = iterator();
		while (pfi.hasNext()) {
			if (pfi.next().prime == p) {
				pfi.previous();
				break;
			}
		}
		int diff = pfi.cursor.pFactor.multiplicity - m;
		if (diff == 0) {
			pfi.remove();
		} else
			pfi.cursor.pFactor.multiplicity -= m;
		return true;
	}
	
	/**
	 * @return size of the list
	 */
	public int size() {
		return size;
	}
	
	/**
	 * Writes out the list as a factorization in the form of a product. Represents exponentiation
	 * by a caret. For example, if the number is 5814, the returned string would be printed out
	 * as "2 * 3^2 * 17 * 19".
	 */
	@Override
	public String toString() {
		String r = "";
		PrimeFactorizationIterator pfi = iterator();
		if (pfi.hasNext())
			r += pfi.next();
		while (pfi.hasNext())
			r += " * " + pfi.next();
		return r;
	}
	
	// the next 3 methods are for testing, but you may use them as you like
	
	/**
	 * @return true if this PrimeFactorization is representing a value that is too large to be within
	 * 				long's range. e.g. 999^999. false otherwise
	 */
	public boolean valueOverflow() {
		return value == OVERFLOW;
	}
	
	/**
	 * @return value represented by this PrimeFactorization, or -1 if valueOverflow()
	 */
	public long value() {
		return value;
	}
	
	public PrimeFactor[] toArray() {
		PrimeFactor[] arr = new PrimeFactor[size];
		int i = 0;
		for (PrimeFactor pf : this)
			arr[i++] = pf;
		return arr;
	}
	
	@Override
	public PrimeFactorizationIterator iterator() {
		return new PrimeFactorizationIterator();
	}
	
	/**
	 * Doubly-linked node type for this class.
	 */
	private class Node {
		public PrimeFactor pFactor; // prime factor
		public Node next;
		public Node previous;
		
		/**
		 * Default constructor for creating a dummy node.
		 */
		public Node() {
			this.pFactor = null;
			this.next = null;
			this.previous = null;
		}
		
		/**
		 * Precondition: p is a prime.
		 * 
		 * @param p - prime number
		 * @param m - multiplicity
		 * @throws IllegalArgumentException if m < 1
		 */
		@SuppressWarnings("unused")
		public Node(int p, int m) throws IllegalArgumentException {
			if (m < 1)
				throw new IllegalArgumentException("The value of m in constructor Node(int p, int m) cannot be less than 1.");
			this.pFactor = new PrimeFactor(p, m);
		}
		
		/**
		 * Constructs a node over a provided PrimeFactor object.
		 * 
		 * @param pf
		 */
		public Node(PrimeFactor pf) {
			this.pFactor = pf;
		}
		
		/**
		 * Printed out in the form: prime + "^" + multiplicity. For instance "2^3".
		 * Also, deal with the case pFactor == null in which a string "dummy" is
		 * returned instead.
		 */
		@Override
		public String toString() {
			String r = "";
			if (this.pFactor == null) {
				r = "dummy";
			} else {
				r = this.pFactor.prime + "^" + this.pFactor.multiplicity;
			}
			return r;
		}
	}
	
	private class PrimeFactorizationIterator implements ListIterator<PrimeFactor> {
		/*
		 * Class invariants:
		 * 1) logical cursor position is always between cursor.previous and cursor
		 * 2) after a call to next(), cursor.previous refers to the node just returned
		 * 3) after a call to previous(), cursor refers to the node just returned
		 * 4) index is always the logical index of node pointed to by cursor
		 */
		
		private Node cursor = head.next;
		private Node pending = null; // node pending for removal
		private int index = 0;
		
		/**
		 * Default constructor positions the cursor before the smallest prime factor.
		 */
		public PrimeFactorizationIterator() {
			// do nothing
		}
		
		@Override
		public boolean hasNext() {
			if (cursor.pFactor != null)
				return true;
			return false;
		}
		
		@Override
		public boolean hasPrevious() {
			if (cursor.previous.pFactor != null)
				return true;
			return false;
		}
		
		@Override
		public PrimeFactor next() {
			PrimeFactor pf = cursor.pFactor;
			index++;
			cursor = cursor.next;
			pending = cursor;
			return pf;
		}
		
		@Override
		public PrimeFactor previous() {
			index--;
			cursor = cursor.previous;
			pending = cursor;
			return cursor.pFactor;
		}
		
		/**
		 * Removes the prime factor returned by next() or previous().
		 * 
		 * @throws IllegalStateException if pending == null
		 */
		@Override
		public void remove() throws IllegalStateException {
			if (pending == null)
				throw new IllegalStateException("Value of pending in PrimeFactorizationIterator cannot be null.");
			cursor = cursor.next;
			pending.previous.next = cursor;
			cursor.previous = pending.previous;
			updateValue();
		}
		
		/**
		 * Adds a prime factor at the cursor position. The cursor is at a wrong position
		 * in either of the two situations below:
		 * 
		 * 		a) pf.prime < cursor.previous.pFactor.prime if cursor.previous != head.
		 * 		b) pf.prime > cursor.pFactor.prime if cursor != tail
		 * 
		 * Take into account the possibility that pf.prime == cursor.pFactor.prime.
		 * 
		 * Precondition: pf.prime is a prime.
		 * 
		 * @param pf
		 * @throws IllegalArgumentException if the cursor is at a wrong position
		 */
		@Override
		public void add(PrimeFactor pf) throws IllegalArgumentException {
			if ((cursor.previous != head && pf.prime < cursor.previous.pFactor.prime) ||
					(cursor != tail && pf.prime > cursor.pFactor.prime))
				throw new IllegalArgumentException("Cursor is at the wrong position.");
			if (cursor.pFactor != null && pf.prime == cursor.pFactor.prime) {
				cursor.pFactor.multiplicity += pf.multiplicity;
			} else {
				Node n = new Node(pf);
				n.previous = cursor.previous;
				n.next = cursor;
				cursor.previous.next = n;
				cursor.previous = n;
				size++;
			}
			updateValue();
			pending = null;
		}
		
		@Override
		public int nextIndex() {
			return index;
		}
		
		@Override
		public int previousIndex() {
			return index - 1;
		}
		
		@Deprecated
		@Override
		public void set(PrimeFactor pf) {
			throw new UnsupportedOperationException(getClass().getSimpleName() + " does not support set method");
		}
	}
	
	/**
	 * Removes all the nodes in the linked list except the two dummy nodes.
	 * 
	 * Made public for testing purposes. Ought to be private otherwise.
	 */
	private void clearList() {
		head.next = tail;
		tail.previous = head;
		updateValue();
	}
	
	/**
	 * Multiply the prime factors (with multiplicities) out to obtain the represented integer.
	 * Use Math.multiply(). If an exception is thrown, assign OVERFLOW to the instance variable value.
	 * Otherwise, assign the multiplication result to the variable.
	 */
	private void updateValue() {
		try {
			value = 1;
			PrimeFactorizationIterator pfi = iterator();
			while (pfi.hasNext()) {
				PrimeFactor pf = pfi.next();
				for (int i = 0; i < pf.multiplicity; i++)
					value = Math.multiplyExact(value, pf.prime);
			}
		} catch (ArithmeticException e) {
			value = OVERFLOW;
		}
	}
}
