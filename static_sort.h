/*
 Copyright (c) 2016 Kang Yue Sheng Benjamin
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef static_sort_h
#define static_sort_h
/*
 Adapted from the Bose-Nelson Sorting network code from:
 https://github.com/atinm/bose-nelson/blob/master/bose-nelson.c
 */

/**
 * A Functor class to create a sort for fixed sized arrays/containers with a
 * compile time generated Bose-Nelson sorting network.
 * \tparam NumElements  The number of elements in the array or container to sort.
 * \tparam T            The element type.
 * \tparam Compare      A comparator functor class that returns true if lhs < rhs.
 */
template <unsigned NumElements, class T, class Compare = void> class StaticSort
{
	template <class A, int I0, int I1, class C> struct Swap
	{
		inline Swap(A &a)
		{
			enum { J0 = I0 - 1, J1 = I1 - 1 };
			T t = Compare()(a[J0], a[J1]) ? a[J0] : a[J1]; // Min
			a[J1] = Compare()(a[J0], a[J1]) ? a[J1] : a[J0]; // Max
			a[J0] = t;
		}
	};
	
	template <class A, int I0, int I1> struct Swap <A, I0, I1, void>
	{
		inline Swap(A &a)
		{
			enum { J0 = I0 - 1, J1 = I1 - 1 };
			T t = a[J0] < a[J1] ? a[J0] : a[J1]; // Min
			a[J1] = a[J0] < a[J1] ? a[J1] : a[J0]; // Max
			a[J0] = t;
		}
	};
	
	template <class A, class C, int I, int J, int X, int Y> struct PB
	{
		inline PB(A &a)
		{
			enum { L = X >> 1, M = (X & 1 ? Y : Y + 1) >> 1, IAddL = I + L, XSubL = X - L };
			PB<A, C, I, J, L, M> p0(a);
			PB<A, C, IAddL, J + M, XSubL, Y - M> p1(a);
			PB<A, C, IAddL, J, XSubL, M> p2(a);
		}
	};
	
	template <class A, class C, int I, int J> struct PB <A, C, I, J, 1, 1>
	{
		inline PB(A &a) { Swap<A, I, J, C> s(a); }
	};
	
	template <class A, class C, int I, int J> struct PB <A, C, I, J, 1, 2>
	{
		inline PB(A &a) { Swap<A, I, J + 1, C> s0(a); Swap<A, I, J, C> s1(a); }
	};
	
	template <class A, class C, int I, int J> struct PB <A, C, I, J, 2, 1>
	{
		inline PB(A &a) { Swap<A, I, J, C> s0(a); Swap<A, I + 1, J, C> s1(a); }
	};
	
	template <class A, class C, int I, int M, bool Stop = false> struct PS
	{
		inline PS(A &a)
		{
			enum { L = M >> 1, IAddL = I + L, MSubL = M - L};
			PS<A, C, I, L, (L <= 1)> ps0(a);
			PS<A, C, IAddL, MSubL, (MSubL <= 1)> ps1(a);
			PB<A, C, I, IAddL, L, MSubL> pb(a);
		}
	};
	
	template <class A, class C, int I, int M> struct PS <A, C, I, M, true>
	{
		inline PS(A &a) {}
	};
	
public:
	/**
	 * Sorts the array/container arr.
	 * \param  arr  The array/container to be sorted.
	 */
	template <class Container> inline void operator() (Container &arr) const
	{
		PS<Container, Compare, 1, NumElements, (NumElements <= 1)> ps(arr);
	};

	/**
	 * Sorts the array arr.
	 * \param  arr  The array to be sorted.
	 */
	inline void operator() (T *arr) const
	{
		PS<T*, Compare, 1, NumElements, (NumElements <= 1)> ps(arr);
	};
};
#endif