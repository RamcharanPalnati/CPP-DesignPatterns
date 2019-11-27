package main
import "fmt"
import "net/http"
import "time"

func runHttpThread(){
	defer func() {
		if errD := recover(); errD != nil {
			
			fmt.Println("Exception Occurred at and Recovered in InitalizeCommunication(), Error Info: ", errD)
		}
	}()
	
	start := time.Now();
	for i:=0;i<2000;i++ {
		resp,_:=http.Get("https://www.facebook.com/");
		fmt.Println(resp)
		resp.Body.Close()
	}
	fmt.Println(time.Since(start))
}

func main(){
	fmt.Println("Hello world");
	go runHttpThread();	
	for{
		time.Sleep(time.Second*30);
	}
    fmt.Println("done")
}