# Introduction #

This page details the structure of data stored on the device profile EEPROM. This structure is created by [bingen](bingen.md) and is different for every controlled device.


# Details #

```
0	int	[ num_menuitems = n
1	menu	[ ico_size = a	num = 1
2	menu	[ ico_size = b	num = 2
3	menu	[ ico_size = c	num = 2
...	...	...
n	menu	[ ico_size = x	num = y
n+1	action	[ type pin value time 
n+2	action	[ type pin value time
...	...	...
j	action	[ type pin value time
k	ico	[ first_menu_icon
k+a	ico	[ second_menu_icon
k+a+b	ico	[ third_menu_icon
...	...	...
z	ico	[ nth_menu_icon



j=n+sizeof(action)*(num_actions-1)
k=j+sizeof(action)
z=EOF-x
```