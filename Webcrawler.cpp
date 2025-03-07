// Webcrawler that handles politeness


#include <chrono>
#include <string>
#include <memory>
#include <queue>
#include <unordered_set>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>
#include <iostream>
#include <random>




using namespace std;






class Threadpool {
    int threads;
    vector<thread> pool;
    queue<function<void()>> tasks;
    bool continue_running;
    mutex m;
    condition_variable cv;
    public:
    explicit Threadpool(int threads):threads(threads),continue_running(true) {
        for(int i = 0; i < threads; i++) {
            pool.push_back(thread([this]() -> void {
                while(1) {
                    unique_lock<mutex> lock(m);
                    cv.wait(lock, [this]() -> bool { return !continue_running or !tasks.empty(); });
                    if (!continue_running) {
                        return;
                    }
                    auto task = tasks.front();
                    tasks.pop();
                    lock.unlock();


                    task();
                }
            }));
        }
    }




    template<typename Func, typename...Args>
    auto addTask(Func func, Args... args) -> future<decltype(func(args...))>
    {
        using return_type = decltype(func(args...));
        auto task = make_shared<packaged_task<return_type()>>(bind(forward<Func>(func), forward<Args>(args)...));
        auto result = task->get_future();


        {
            lock_guard<mutex> lock(m);
            tasks.emplace([task]() -> void {
                (*task)();
            });
            cv.notify_one();
        }




        return result;
    }




    ~Threadpool() {
        {
            lock_guard<mutex> lock(m);
            continue_running = false;
            cv.notify_all();
        }


        for(auto &t : pool) t.join();
    }


};


class Url {
    public:
    string url;
    int depth;
    Url(const string &url):url(url), depth(0) {}
    bool operator==(const Url& other) const {
        return other.depth == depth and other.url == url;
    };
    string data() const {
        this_thread::sleep_for(chrono::milliseconds(100));
        return "Parsed";


    }
};


struct Hash {
    size_t operator()(const Url& url) const {
        return hash<string>{}(url.url + to_string(url.depth));
    }
};


class WebCrawler {
    queue<Url> frontier;
    unique_ptr<Threadpool> pool;
    bool state;
    int limit;


    mutex m, m2;
    condition_variable cv;
    vector<Url> urls;
    unordered_map<Url, vector<Url>, Hash> webData;
    unordered_map<string, string> crawledData;
    public:
    unordered_set<Url, Hash> visited;
    WebCrawler(vector<Url> &seeds, unique_ptr<Threadpool> pool, vector<Url> &urls, unordered_map<Url, vector<Url>, Hash> &webData):pool(std::move(pool)),state(true),limit(150),urls(urls), webData(webData) {
        for(auto &url: seeds) {
            frontier.push(url);
        }
    }




    void crawl() {
        auto task = make_shared<function<void(const Url&)>>([this](const Url &url) -> void {
            {
                lock_guard<mutex> lock(m2);
                cout << this_thread::get_id() << " parsing data " << visited.size() << endl;
                visited.insert(url);
                if (visited.size() == limit) {
                    state = false;
                }
                crawledData[url.url] = url.data();
            }
            cv.notify_all();
            {
                lock_guard<mutex> lock(m2);
                bool pushed_some = false;
                for(auto &ch: webData[url])  {
                    if (visited.find(ch) != visited.end()) continue;
                    frontier.push(ch);
                    pushed_some = true;
                }
            }
            cv.notify_one();
        });
        while(1) {
            unique_lock lock(m);
            cv.wait(lock, [this]() -> bool { return visited.size() >= limit or !frontier.empty();});
            if (!state) {
                return;
            }
            if (!frontier.empty()) {
                auto url = frontier.front();
                frontier.pop();
                lock.unlock();
                pool->addTask(*task, url);
            }
        }
        // Add till the frontier queue is not empty
        // If it is empty then wait for some time and then listen to the queue again,
        // If it is not empty add a new task to make a request to the url and get the text.
        // Store the text in an map, mapping link to text.
       




        // Appended Task
        // 1. Check if the depth is not greater than the required depth
        // 2. Check if throttling required. If required make this sleep for some time before starting crawling again.
    }
};


// Function to generate a random URL
std::string generateRandomUrl() {
    const std::vector<std::string> domains = {".com", ".net", ".org", ".io", ".tech"};
    const std::string chars = "abcdefghijklmnopqrstuvwxyz";


    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> len_dist(5, 10); // Length of website name
    std::uniform_int_distribution<> char_dist(0, chars.size() - 1);
    std::uniform_int_distribution<> domain_dist(0, domains.size() - 1);


    std::string site = "https://";
    int name_length = len_dist(gen);
   
    for (int i = 0; i < name_length; ++i) {
        site += chars[char_dist(gen)];
    }
    site += domains[domain_dist(gen)];


    return site;
}


pair<vector<Url>, unordered_map<Url, vector<Url>, Hash>> createWebData() {
    constexpr int TOTAL_URLS = 200;
    std::unordered_map<Url, std::vector<Url>, Hash> urlGraph;
    std::vector<Url> urls;


    // Generate unique URLs
    while (urls.size() < TOTAL_URLS) {
        std::string newUrl = generateRandomUrl();
        if (std::find(urls.begin(), urls.end(), newUrl) == urls.end()) {
            urls.push_back(Url(newUrl));
        }
    }


    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> link_count_dist(1, 5); // Number of outgoing links


    int totalLinks = 0;


    // Populate URL mapping
    for (const auto& url : urls) {
        int linkCount = link_count_dist(gen);
        std::vector<Url> links;
       
        for (int i = 0; i < linkCount; ++i) {
            links.push_back(Url(urls[gen() % TOTAL_URLS]));
        }


        urlGraph[url] = links;
        totalLinks += linkCount;
    }


    // Display results
    for (const auto& [key, value] : urlGraph) {
        std::cout << key.url << " links to:\n";
        for (const auto& link : value) {
            std::cout << "  - " << link.url << "\n";
        }
    }


    std::cout << "\nTotal number of links created: " << totalLinks << "\n";


    return {urls, urlGraph};
}


int main () {


    auto pool = make_unique<Threadpool>(10);
    auto [urls, urlGraph] = createWebData();
    vector<Url> seeds;
    std::uniform_int_distribution<int> dist(0, urls.size() - 1);


    std::random_device rd;
    std::mt19937 gen(rd());
    urlGraph[Url("base")] = urls;
    seeds.push_back(Url("base"));


    WebCrawler crawler(seeds, std::move(pool), urls, urlGraph);


    crawler.crawl();


    cout << "Parsed " << crawler.visited.size() << " websites" << endl;




}
