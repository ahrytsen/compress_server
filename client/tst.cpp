// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   tst.cpp                                            :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: ahrytsen <ahrytsen@student.unit.ua>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2018/10/03 16:01:11 by ahrytsen          #+#    #+#             //
//   Updated: 2018/10/03 16:26:01 by ahrytsen         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include <iostream>
#include <thread>
#include <functional>
#include <unistd.h>

void handler(int i) {
	sleep(i);
	std::cout << "lolkek: " << i << std::endl;
}
int	main( void ) {
	for (int i = 5; i < 10; i++){
		std::thread t(std::bind(handler, i));
		t.detach();
	}
	sleep(20);
}
