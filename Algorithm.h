#ifndef _ALGORITHM_H_
#define _ALGORITHM_H_

#include <cstring>
#include <utility>

#include "Allocator.h"
//#include "Functional.h//还没写，先注释掉
#include "Iterator.h"
#include "TypeTraits.h"
//#include "Utility.h"//还没写，先注释掉

namespace WhoseTinySTL{
    //********** [distance] ******************************
	//********* [Algorithm Complexity: O(N)] ****************
	template<class InputIterator>
	typename iterator_traits<InputIterator>::difference_type
		_distance(InputIterator first, InputIterator last, input_iterator_tag){
		typename iterator_traits<InputIterator>::difference_type dist = 0;
		while (first++ != last){
			++dist;
		}
		return dist;
	}
	template<class RandomIterator>
	typename iterator_traits<RandomIterator>::difference_type
		_distance(RandomIterator first, RandomIterator last, random_access_iterator_tag){
		auto dist = last - first;
		return dist;
	}
	template<class Iterator>
	typename iterator_traits<Iterator>::difference_type
		distance(Iterator first, Iterator last){
		typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
		return _distance(first, last, iterator_category());
	}
    //********** [advance] ******************************
	//********* [Algorithm Complexity: O(N)] ****************
	namespace {
		template<class InputIterator, class Distance>
		void _advance(InputIterator& it, Distance n, input_iterator_tag){
			assert(n >= 0);
			while (n--){
				++it;
			}
		}
		template<class BidirectionIterator, class Distance>
		void _advance(BidirectionIterator& it, Distance n, bidirectional_iterator_tag){
			if (n < 0){
				while (n++){
					--it;
				}
			}else{
				while (n--){
					++it;
				}
			}
		}
		template<class RandomIterator, class Distance>
		void _advance(RandomIterator& it, Distance n, random_access_iterator_tag){
			if (n < 0){
				it -= (-n);
			}else{
				it += n;
			}
		}
	}
	template <class InputIterator, class Distance> 
	void advance(InputIterator& it, Distance n){
		typedef typename iterator_traits<InputIterator>::iterator_category iterator_category; // 作者这行少打了typename，不然是识别不出来这是类型的
		_advance(it, n, iterator_category());
	}
}

#endif