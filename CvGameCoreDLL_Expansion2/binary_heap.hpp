// Heap Plus implementation 1.01alpha
// ChangeLog:
//  2010.03.19  - Updated __down_heap (Yanbin Lu)
//  2009.09.26  - Updated __down_heap, added _updatepos (Yanbin Lu)
//  2009.08.10  - Added update_heap_pos functions so that we can adjust
//                heap values outside of update_heap functions -- e.g., if
//                we have external pointers into the heap entries -- then
//                call update_heap on the position only, regardless of the
//                value.
//                (Danny Tarlow: dtarlow@cs.toronto.edu)
//  2006.12.18  - Initially created (lihw)
 
#ifndef BINARY_HEAP_HPP
#define BINARY_HEAP_HPP
 
#include <iterator>
 
namespace std {
   template<typename _RandomAccessIterator, typename _Distance, typename _Tp,
      typename _Compare, typename _Updatepos>
      void
      __push_heap(_RandomAccessIterator __first, _Distance __holeIndex,
                  _Distance __topIndex, _Tp __value, _Compare __comp,
                  _Updatepos __updatepos)
   {
      _Distance __parent = (__holeIndex - 1) / 2;
      while (__holeIndex > __topIndex
             && __comp(*(__first + __parent), __value))
      {
         *(__first + __holeIndex) = *(__first + __parent);
         __updatepos(*(__first + __holeIndex), __holeIndex); /* yanbin */
         __holeIndex = __parent;
         __parent = (__holeIndex - 1) / 2;
      }
      *(__first + __holeIndex) = __value;
      __updatepos(*(__first + __holeIndex), __holeIndex); /* yanbin */
   }

   template<typename _RandomAccessIterator, typename _Compare, typename _Updatepos>
      inline void
      push_heap(_RandomAccessIterator __first, _RandomAccessIterator __last,
                _Compare __comp, _Updatepos __updatepos)
   {
      typedef typename iterator_traits<_RandomAccessIterator>::value_type
         _ValueType;
      typedef typename iterator_traits<_RandomAccessIterator>::difference_type
         _DistanceType;

      // concept requirements
      __glibcxx_function_requires(_Mutable_RandomAccessIteratorConcept<
                                  _RandomAccessIterator>)
         __glibcxx_requires_valid_range(__first, __last);
      __glibcxx_requires_heap_pred(__first, __last - 1, __comp);

      std::__push_heap(__first, _DistanceType((__last - __first) - 1),
		       _DistanceType(0), _ValueType(*(__last - 1)), __comp, __updatepos);
   }

   template<typename _RandomAccessIterator, typename _Distance,
      typename _Tp, typename _Compare, typename _Updatepos>
      void
      __adjust_heap(_RandomAccessIterator __first, _Distance __holeIndex,
                    _Distance __len, _Tp __value, _Compare __comp, _Updatepos __updatepos)
   {
      const _Distance __topIndex = __holeIndex;
      _Distance __secondChild = 2 * __holeIndex + 2;
      while (__secondChild < __len)
      {
         if (__comp(*(__first + __secondChild),
                    *(__first + (__secondChild - 1))))
            __secondChild--;
         *(__first + __holeIndex) = *(__first + __secondChild);
         __updatepos(*(__first + __holeIndex), __holeIndex); /* yanbin */
         __holeIndex = __secondChild;
         __secondChild = 2 * (__secondChild + 1);
      }
      if (__secondChild == __len)
      {
         *(__first + __holeIndex) = *(__first + (__secondChild - 1));
         __updatepos(*(__first + __holeIndex), __holeIndex); /* yanbin */
         __holeIndex = __secondChild - 1;
      }
      std::__push_heap(__first, __holeIndex, __topIndex, __value, __comp, __updatepos);
   }

   template<typename _RandomAccessIterator, typename _Tp, typename _Compare, typename _Updatepos>
      inline void
      __pop_heap(_RandomAccessIterator __first, _RandomAccessIterator __last,
                 _RandomAccessIterator __result, _Tp __value, _Compare __comp, _Updatepos __updatepos)
   {
      typedef typename iterator_traits<_RandomAccessIterator>::difference_type
         _Distance;
      *__result = *__first;
      std::__adjust_heap(__first, _Distance(0), _Distance(__last - __first),
			 __value, __comp, __updatepos);
   }

   template<typename _RandomAccessIterator, typename _Compare, typename _Updatepos>
      inline void
      pop_heap(_RandomAccessIterator __first,
               _RandomAccessIterator __last, _Compare __comp, _Updatepos __updatepos)
   {
      // concept requirements
      __glibcxx_function_requires(_Mutable_RandomAccessIteratorConcept<
                                  _RandomAccessIterator>)
         __glibcxx_requires_valid_range(__first, __last);
      __glibcxx_requires_heap_pred(__first, __last, __comp);

      typedef typename iterator_traits<_RandomAccessIterator>::value_type
         _ValueType;
      std::__pop_heap(__first, __last - 1, __last - 1,
		      _ValueType(*(__last - 1)), __comp, __updatepos);
   }

   template<typename _RandomAccessIterator, typename _Compare, typename _Updatepos>
      inline void
      make_heap(_RandomAccessIterator __first, _RandomAccessIterator __last,
                _Compare __comp, _Updatepos __updatepos)
   {
      typedef typename iterator_traits<_RandomAccessIterator>::value_type
         _ValueType;
      typedef typename iterator_traits<_RandomAccessIterator>::difference_type
         _DistanceType;

      // concept requirements
      __glibcxx_function_requires(_Mutable_RandomAccessIteratorConcept<
                                  _RandomAccessIterator>)
         __glibcxx_requires_valid_range(__first, __last);

      if (__last - __first < 2)
      {
         for (_DistanceType __len = 0; __len < __last - __first; __len ++)
         {
            __updatepos(*(__first + __len), __len); /* yanbin */
         }

         return;
      }
      const _DistanceType __len = __last - __first;
      _DistanceType __parent = (__len - 2) / 2;
      while (true)
      {
         std::__adjust_heap(__first, __parent, __len,
                            _ValueType(*(__first + __parent)), __comp, __updatepos);
         if (__parent == 0)
         {
            for (_DistanceType __len = 0; __len < __last - __first; __len ++)
            {
               __updatepos(*(__first + __len), __len); /* yanbin */
            }

            return;
         }
         __parent--;
      }

   }


   template<typename _RandomAccessIterator, typename _Distance, typename _Tp, typename _Compare, typename _Updatepos>
      inline void
      __up_heap(_RandomAccessIterator __first, _RandomAccessIterator __end, _RandomAccessIterator __pos,
                _Distance, _Tp __value,  _Compare __comp, _Updatepos __updatepos)
   {
      _Distance __parent = (__pos - __first - 1) /  2;
      _Distance __index = __pos - __first;
      while (__index > 0 && __comp(*(__first + __parent), __value)) {
         *(__first + __index) = *(__first + __parent);
         __updatepos(*(__first + __index), __index); /* yanbin */

         __index = __parent;
         __parent = (__parent - 1) / 2;
      }
        
      if (__pos != (__first + __index)) {
         *(__first + __index) = __value;
         __updatepos(*(__first + __index), __index); /* yanbin */
      }
   }
 
   template<typename _RandomAccessIterator, typename _Distance, typename _Tp, typename _Compare, typename _Updatepos>
      inline void
      __down_heap(_RandomAccessIterator __first, _RandomAccessIterator __last, _RandomAccessIterator __pos,
                  _Distance, _Tp __value, _Compare __comp, _Updatepos __updatepos)
   {
      _Distance __len = __last - __first;
      _Distance __index = __pos - __first;
      _Distance __left = __index * 2 + 1;
      _Distance __right = __index * 2 + 2;
      _Distance __largest;
      while (__index < __len) 
      {
         if (__right < __len)
         {
            if (__comp(*(__first + __left), *(__first + __right)))
            {
               __largest = __right;
            }
            else
            {
               __largest = __left;
            }
         }
         else if (__left < __len)
         {
            __largest = __left;
         }
         else
         {
            __largest = __index;
         }

         if (__largest < __len && __comp(__value, *(__first + __largest))) 
         {
            *(__first + __index) = *(__first + __largest);
            __updatepos(*(__first + __index), __index); /* yanbin */
            __index = __largest;
            *(__first + __index) = __value; /* yanbin */
            __left = __index * 2 + 1;
            __right = __index * 2 + 2;
         } else
            break;
      }
 
      if (__pos != (__first + __index)) {
         *(__first + __index) = __value;
         __updatepos(*(__first + __index), __index); /* yanbin */
      }
   }
 
   template<typename _RandomAccessIterator, typename _Distance, typename _Tp,
      typename _Compare, typename _Updatepos>
      inline void
      __update_heap(_RandomAccessIterator __first, _RandomAccessIterator __last,
                    _RandomAccessIterator __pos, _Distance, _Tp __value, _Compare __comp, _Updatepos __updatepos)
   {
      *(__pos) = __value;
     
      _Distance __index = (__pos - __first);
      _Distance __parent = (__index - 1) / 2;
 
      if (__index > 0 && __comp(*(__first + __parent), __value))
         __up_heap(__first, __last, __pos, _Distance(), __value, __comp, __updatepos);
      else
         __down_heap(__first, __last, __pos, _Distance(), __value, __comp, __updatepos);
   }
 
   template<typename _RandomAccessIterator, typename _Distance, typename _Compare, typename _Updatepos>
      inline void
      __update_heap(_RandomAccessIterator __first, _RandomAccessIterator __last,
                    _RandomAccessIterator __pos, _Distance, _Compare __comp, _Updatepos __updatepos)
   {
      _Distance __index = (__pos - __first);
      _Distance __parent = (__index - 1) / 2;
      if (__index > 0 && __comp(*(__first + __parent), *(__pos)))
         __up_heap(__first, __last, __pos, _Distance(), *(__pos), __comp, __updatepos);
      else
         __down_heap(__first, __last, __pos, _Distance(), *(__pos), __comp, __updatepos);
   }
 
   template<typename _RandomAccessIterator, typename _Tp, typename _Updatepos>
      inline void
      update_heap(_RandomAccessIterator __first, _RandomAccessIterator __last,
                  _RandomAccessIterator __pos, _Tp __value, _Updatepos __updatepos)
   {
      typedef typename iterator_traits<_RandomAccessIterator>::value_type _ValueType;
      typedef typename iterator_traits<_RandomAccessIterator>::difference_type _DistanceType;
 
      __update_heap(__first, __last, __pos, _DistanceType(), __value, less<_ValueType>(), __updatepos);
   }
 
   template<typename _RandomAccessIterator, typename _Tp, typename _Compare, typename _Updatepos>
      inline void
      update_heap(_RandomAccessIterator __first, _RandomAccessIterator __last,
                  _RandomAccessIterator __pos, _Tp __value, _Compare __comp, _Updatepos __updatepos)
   {
      __update_heap(__first, __last, __pos, __value, __comp, __updatepos);
   }
 
 
   template<typename _RandomAccessIterator, typename _Updatepos>
      inline void
      update_heap_pos(_RandomAccessIterator __first, _RandomAccessIterator __last,
                      _RandomAccessIterator __pos, _Updatepos __updatepos) {
      typedef typename iterator_traits<_RandomAccessIterator>::value_type _ValueType;
      typedef typename iterator_traits<_RandomAccessIterator>::difference_type _DistanceType;
 
      __update_heap(__first, __last, __pos, _DistanceType(), less<_ValueType>(), __updatepos);
   }
 
 
   template<typename _RandomAccessIterator, typename _Compare, typename _Updatepos>
      inline void
      update_heap_pos(_RandomAccessIterator __first, _RandomAccessIterator __last,
                      _RandomAccessIterator __pos, _Compare __comp, _Updatepos __updatepos) {
      typedef typename iterator_traits<_RandomAccessIterator>::value_type _ValueType;
      typedef typename iterator_traits<_RandomAccessIterator>::difference_type _DistanceType;

      __update_heap(__first, __last, __pos, _DistanceType(), __comp, __updatepos);
   }
 
 
 
}; // namespace std
 
#endif // !BINARY_HEAP_HPP
