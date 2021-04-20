Partners: 
Torrance Cui and Veltson Bastien

Both programs work by taking in the input passed in by the respective clients (TCP, or UDP) and repeatedly sums (and returns that answer to the client) until the sum is a single digit. In the case that the inputted text was empty, or contained letters, an error message would be returned. 

For the TCP branch a delay() function was needed because messages were not being sent over with the proper spacing. This was due to the buffer on the client side not having enough time to clear. By adding in the delays, this allowed for a proper clearing of the buffer and the output would display as expected. 


