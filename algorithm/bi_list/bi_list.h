#ifndef __ALGORITHM_BILIST_BILIST_H__
#define __ALGORITHM_BILIST_BILIST_H__

#include <cassert>
#include <string>
namespace BGIQD{
    namespace BILIST{

        template<class T>
            struct BiList
            {
                typedef BiList * BiListPtr ;
                typedef T *      ContainerPtr;
                BiListPtr left;
                BiListPtr right;
                T * self;

                void Init(T * s) 
                {
                    left = this;
                    right = this;
                    self = s ;
                }

                // first
                //   * assume this as the root of bi-list ,
                //   * assume left is the forward order ,
                //   * so that the right node of this is the end of bi-list ,
                //
                // second
                //
                //   * always insert node at end of bi-list ,
                // so
                //   let node be the left of this's right node !
                void Insert( BiListPtr node)
                {
                    assert( node != NULL );
                    assert( left != NULL && right != NULL );
                    assert( node->left != NULL && node->right != NULL );

                    auto A = this ;
                    auto B = node ;
                    auto ar = A->right ;
                    auto br = B->right ;
                    A->right->left = B ;
                    B->right->left = A ;
                    A->right = br ;
                    B->right= ar ;
                }

                void DeleteMe() 
                {
                    assert( left != NULL && right != NULL );
                    if( left != this  && right != this )
                    {
                        left->right = right ;
                        right->left = left ;
                    }
                    else
                    {
                        assert( left == this && right == this );
                    }
                    Init(self);
                }

                bool Single() const 
                {
                    return left == this && right == this ;
                }

                BiListPtr Forward()
                {
                    return left;
                }

                BiListPtr Backword()
                {
                    return right;
                }

            };

    }
}
#endif //__ALGORITHM_BILIST_BILIST_H__
