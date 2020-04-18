# deonSTL

自己实现的STL，since c++11

| 组件          |            |
| ------------- | ---------- |
| allocater     | ✅          |
| constructor   | ✅          |
| iterator      | ✅          |
| type_traits   | ✅          |
| uninitialized | ✅          |
| util          | pair待实现 |

| 容器    |      |
| ------- | ---- |
| vector  | ✅    |
| deque   | ✅    |
| stack   | ✅    |
| rb_tree | 🔧    |
| set     |      |
| map     |      |

### rb_tree

| 内容                 |
| -------------------- |
| rb_tree_traits       |
| rb_tree_value_traits |
| rb_tree_node         |

| rb_tree_iter/rb_tree_const_iter |
| ------------------------------- |
| 构造/赋值/移动/析构             |
| */->                            |
| ++,--                           |
| ==,!=                           |

| tree_algorithms          |
| ------------------------ |
| ...                      |
| rb_tree_insert_rebalance |
| rb_tree_erase_rebalance  |

| rb_tree                                             |                                                         |
| --------------------------------------------------- | ------------------------------------------------------- |
| header_                                             | parent -> root，<br />left -> lmost<br />right -> rmost |
| emplace_multi(...args)<br />emplace_unique(...args) | 接受参数包，构造插入                                    |
| Insert_multi(value)<br />insert_unique(value)       | 插入，满足可移动插入                                    |
| find(key)                                           | 寻找key位置                                             |
| count_multi(key)<br />count_unique(key)             | 查询key个数                                             |
| lower_bound(key)                                    | 查询大于等于key的第一个元素                             |
| upper_bound(key)                                    | 查询大于key的第一个元素                                 |
| equal_range(key)                                    | 返回pair(first, last)，表示key的范围                    |

