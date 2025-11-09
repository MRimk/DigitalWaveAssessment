#include <functional>
#include <iostream>
#include <curl/curl.h>
#include "QuickHashMap.hpp"


const std::string BOOK_URL = "https://www.gutenberg.org/files/98/98-0.txt";

struct CurlContext {
    QuickHashMap* map;
    std::string leftover;
};

static void handleWord(QuickHashMap* map, const std::string& word){
    int curr = map->get(word);
    map->insert(word, curr + 1);
    std::cout << "WORD: " << word << ", current: " << curr << '\n';
}

static size_t write_to_stream(void *ptr, size_t size, size_t nmemb, void *u_data) {
    const size_t total = size * nmemb;
    const char*  data  = static_cast<const char*>(ptr);

    CurlContext* ctx = static_cast<CurlContext*>(u_data); // unpack context

    for (size_t i = 0; i < total; ++i) {
        char c = data[i];
        // build word
        if (std::isalpha(static_cast<unsigned char>(c))) {
            ctx->leftover.push_back(static_cast<char>(std::tolower(c)));
        } else {
            // apply action on word
            if (!ctx->leftover.empty()) {
                handleWord(ctx->map, ctx->leftover);
                ctx->leftover.clear();
            }
        }
    }
    return total;                         
}

// using the tutorial from libcurl: https://curl.se/libcurl/c/CURLOPT_WRITEFUNCTION.html
void downloadText(QuickHashMap* wordFrequencies){
    CURL *curl;

    CurlContext ctx {wordFrequencies, {}};
 
    CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
    if(res)
        return;
    
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, BOOK_URL.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_stream);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ctx);   
    
        /* Perform the request, res gets the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
    
        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

}

int main() {
#ifdef QUICK_TEST
    QuickHashMap map(300);
    map.insert("hello", 1);
    map.insert("world", 2);
    map.insert("foo", 3);
    map.insert("hello", 4);

    std::cout << "hello : " << map.get("hello") << '\n'; // 4
    std::cout << "world : " << map.get("world") << '\n'; // 2
    std::cout << "getLast : " << map.getLast() << '\n'; // hello
    std::cout << "getFirst : " << map.getEarliest() << '\n'; // world

    map.remove("world");
    std::cout << "after remove(world), getFirst : " << map.getEarliest() << '\n'; // foo
#else
    QuickHashMap wordMap(10'000);
    downloadText(&wordMap);
#endif
}