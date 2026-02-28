#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>
#include <vector>
#include <unordered_map>
#include <limits>
using namespace std;
namespace fs = std::filesystem;
using json = nlohmann::json;
struct snap{
    string task_id;
    string task_name;
    string task_status;
    string task_note;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    snap,
    task_id,
    task_name,
    task_status,
    task_note
)
namespace wk{
    class Persistance{
        protected:
            static unordered_map<string,unordered_map<string,string>> user_data; // {user_id : {user data...}}
            static unordered_map<string,unordered_map<string,snap>> user_task; //stores data for the tasks per user
            static unordered_map<string,unordered_map<string,vector<snap>>> history; // user_id : {task_id : {vector......}}
        public:
            void create_file(){
                json j;
                j["user_data"] = json::object();
                j["user_task"] = json::object();
                j["history"] = json::object();
                ofstream file("Metadata.json");
                file << j.dump(4);
                file.close();
            }

            bool check_file_exist(){
                if(!fs::exists("Metadata.json")){
                    create_file();
                    return false;
                }
                else{
                    return true;
                }
            }

            void write_data_to_json(){
                if(check_file_exist() == true){
                    try{
                        cout << "Writing to JSON...\n";
                        json j;
                        j["user_data"] = user_data;
                        j["user_task"] = user_task;
                        j["history"] = history;
                        ofstream file("Metadata.json");
                        file << j.dump(4);
                        cout<<"Successfully written in json...\n\n";
                        file.close();

                    }
                    catch(const std::exception& e){
                        cout<<"Error : "<<e.what()<<endl;
                        return;
                    }
                }
                else{
                    cout<<"Sorry the metadata was not available, created a new copy. Kindly retry...\n";
                    return;
                }
            }
            void restore_data(){
                if(check_file_exist() == true){
                    ifstream file("Metadata.json");
                    if(!file.is_open()){
                        cout<<"Some error occured...\n";
                        return;
                    }
                    try{

                        json j;
                        file >>j;
                        file.close();
                        user_data = j["user_data"].get<unordered_map<string,unordered_map<string,string>>>();
                        user_task = j["user_task"].get<unordered_map<string,unordered_map<string,snap>>>();
                        history = j["history"].get<unordered_map<string,unordered_map<string,vector<snap>>>>();
                    }
                    catch(const std::exception& e){
                        cout<<"Error : "<<e.what()<<endl;
                        return;
                    }

                }
                else{
                    cout<<"Sorry the metadata was not available, created a new copy. Kindly retry...\n";
                    return;
                }
            }

    };

    class User : public Persistance{
        protected : 
            bool user_exist(const string& user_id){
                if(user_data.find(user_id) != user_data.end()){
                    //user found
                    return true;
                }
                else{
                    //user not found
                    return false;
                }
            }
        public :
            snap tk;
            static vector<string> task_status;
            User(){}
            User(string user_name,string user_gender, string user_age, string user_email,string user_id){
                if(user_exist(user_id) == true){
                    cout<<"Sorry the user already exists...\n";
                    return;
                }
                else{
                    this->user_data[user_id]["Name"] = user_name;
                    this->user_data[user_id]["Gender"] = user_gender;
                    this->user_data[user_id]["Age"] = user_age;
                    this->user_data[user_id]["Email"] = user_email;
                    write_data_to_json();
                    cout<<"User created successfully.....\nWelcome "<<user_name<<endl;
                }
            }

            void remove_user(const string& user_id){
                if(user_exist(user_id) == false){
                    cout<<"Sorry the user does not exist...\n";
                    return;
                }
                else{
                    int choice;
                    cout<<"Are you sure to remove this user ? \n1.Yes \n2.No \nYour response : ";
                    cin>>choice;
                    if(choice == 1){
                        
                        user_data.erase(user_id);
                        write_data_to_json();
                        cout<<"User removed successfully....\n";
                        return;
                    }    
                    else if(choice == 2){
                        cout<<"Aborting...\n";
                        return;
                    }
                    else{
                        cout<<"Please enter a valid option...\n";
                        return;
                    }
                }
            }

            void view_all_users(){
                for (const auto& userPair : user_data) { //printing data from user_data through for each loop, auto& means uuserPair is a referencing to user_data and type is automatically detected
                        string user_id = userPair.first;
                        const auto& fields = userPair.second;
                        cout << "User ID: " << user_id << '\n';

                        for (const auto& fieldPair : fields) {
                            cout << "  " << fieldPair.first << " : " << fieldPair.second << '\n';
                        }

                        cout << "----------------------\n";
                }
            }

            void view_specific_user(const string& user_id){
                if(user_exist(user_id) == false){
                    cout<<"Sorry the use does not exist...\n";
                    return;
                }
                else{
                    cout<<"User Id : "<<user_id<<endl;
                    for(const auto& pair : user_data.at(user_id)){
                        cout<<"  "<<pair.first<<" : "<<pair.second<<endl;
                    }
                }
            }



    };
    class Task_function : public User{

        protected:
            bool user_in_history(const string& user_id){
                if(history.find(user_id)  != history.end()){
                    return true;
                }
                else{
                    return false;
                }
            }
            bool user_exist_in_user_task(const string& user_id,const string& task_id){
                auto userIt = user_task.find(user_id);
                if (userIt == user_task.end()){
                    
                    return false;   // user has no tasks
                }
                else{
                    return true; //user has atleast one task
                }
            }
            bool usertask_exist_in_user_task(const string& user_id,const string& task_id){
                auto userIt = user_task.find(user_id);
                if(userIt == user_task.end()){
                    return false;
                }
                if (userIt->second.find(task_id) != userIt->second.end()){
    
                    return true;   //specific task exists
                }
                else{
    
                    return false;   //specific task does not exist
                }
            }
            //checking if the task of the user exist in history or not
            bool check_history_task(const string& user_id,const string& task_id){
                auto userIt = history.find(user_id);
                if (userIt == history.end()){
                    
                    return false;   // user has no tasks in history
                }

                if (userIt->second.find(task_id) != userIt->second.end()){

                    return true;   //specific task exists in history
                }
                else{

                    return false;   //specific task does not exist in history
                }
            }





    };
    class Task : public Task_function{
        public :
            Task(){}
            Task(string user_name,string user_gender,string user_age,string user_email, string user_id) {User(user_name,user_gender,user_age,user_email,user_id);}

            void take_snap(const string& user_id,const string& task_id){
                if(user_exist(user_id) == true && user_exist_in_user_task(user_id,task_id) == true && usertask_exist_in_user_task(user_id,task_id) == true){
                
                    auto userIt = user_task.find(user_id);
                    if (userIt == user_task.end()) return;

                    auto taskIt = userIt->second.find(task_id);
                    if (taskIt == userIt->second.end()) return;

                    history[user_id][task_id].push_back(taskIt->second);
            

                    cout << "Saved in memory...\n"; 

                }
                else{
                    cout<<"Enter correct combination of user and task id..."<<endl;
                    return;
                }
            }
            void view_history(const string& user_id,const string& task_id){
                if(check_history_task(user_id,task_id) == true){
                    const auto& user = history.find(user_id);
                    const auto& task = user->second;
                    cout<<"User Id : "<<user_id<<endl;
                    for(const auto& task_pair : task){
                        string task_id = task_pair.first;
                        const vector<snap>& versions = task_pair.second;
                        cout<<"Task id : "<<task_id<<endl;
                        for(size_t version_index = 0;version_index < versions.size();++version_index){
                            const snap& s = versions[version_index];
                            cout << "    Version Index: " << version_index << "\n";
                            cout << "      Name   : " << s.task_name << "\n";
                            cout << "      Status : " << s.task_status << "\n";
                            cout << "      Note   : " << s.task_note << "\n";
                        }

                    }

                    
                }
            }

            void roll_back(const string& user_id, const string& task_id){
                if(user_in_history(user_id) == true){
                    
                    cout<<"All the tasks are : "<<endl;
                    int choice;
                    view_history(user_id,task_id);
                    cout<<"Enter the version you want to roll back to : ";
                    cin>>choice;
                    if(choice >=0 && choice < history.at(user_id).at(task_id).size()){
                        user_task[user_id][task_id] = history[user_id][task_id][choice];
                        write_data_to_json();
                        cout<<"Version restored..."<<endl;
                    }
                    else{
                        cout<<"Sorry that index does not exist, please try a valid index..."<<endl;
                        return;
                    }
                }
                else{
                    cout<<"Sorry the combination of user id and task id were invalid.."<<endl;
                    return;
                }
            }
            void create_task(const string& user_id,const string& task_id,const string& task_title, const string& task_status, const string& task_note){
                if(user_exist(user_id) == true && usertask_exist_in_user_task(user_id,task_id) == false){
                    tk.task_id = task_id;
                    tk.task_name = task_title;
                    tk.task_status = task_status;
                    tk.task_note = task_note;
                    user_task[user_id][task_id] = tk;
                    take_snap(user_id,task_id);
                    write_data_to_json();
                    cout<<"Task added successfully..."<<endl;
                }
            }
            void remove_task(const string& user_id,const string& task_id){
                if(user_exist(user_id) == true && usertask_exist_in_user_task(user_id,task_id) == true){
                    string choice;
                    cout<<"Are you sure to delete the task ? | y/n : ";
                    getline(cin,choice);
                    if(choice == "y" || choice == "Y"){
                        user_task.at(user_id).erase(task_id);
                        take_snap(user_id,task_id);
                        write_data_to_json();
                        cout<<"Task deleted successfully....."<<endl;
                    }
                    else if(choice == "n" || choice == "N"){
                        cout<<"Aborting..."<<endl;
                        return;
                    }
                    else{
                        cout<<"Enter a valid option...."<<endl;
                        return;
                    }
                }
            }
            void change_task_status(const string& user_id,const string& task_id,int choice){
                if(usertask_exist_in_user_task(user_id,task_id) == true){ // 1 = task_status[0] -> TO-DO, 2 = task_status[1] -> IN_PROGRESS, 3 = task_status[3] -> COMPLETED
                    if(choice == 1){
                        user_task.at(user_id).at(task_id).task_status = task_status[0];
                        take_snap(user_id,task_id);
                        write_data_to_json();
                        cout<<"Task status changed successfully..."<<endl;
                    }
                    else if(choice == 2){
                        user_task.at(user_id).at(task_id).task_status = task_status[1];
                        take_snap(user_id,task_id);
                        write_data_to_json();
                        cout<<"Task status changed successfully..."<<endl;
                    }
                    else if(choice == 3){
                        user_task.at(user_id).at(task_id).task_status = task_status[2];
                        take_snap(user_id,task_id);
                        write_data_to_json();
                        cout<<"Task status changed successfully..."<<endl;
                    }
                    else{
                        cout<<"Please enter a valid choice..."<<endl;
                        return;
                    }
                }
            }
            void change_task_note(const string& user_id,const string& task_id,string note){
                if(usertask_exist_in_user_task(user_id,task_id) == true){
                    user_task.at(user_id).at(task_id).task_note = note;
                    take_snap(user_id,task_id);
                    write_data_to_json();
                    cout<<"Note changed successfully..."<<endl;
                }
                else{
                    cout<<"The combination of user id and task id is invalid..."<<endl;
                    return;
                }
            }
            void change_task_title(const string& user_id,const string& task_id,string title){
                if(usertask_exist_in_user_task(user_id,task_id)==true){
                    user_task.at(user_id).at(task_id).task_name = title;
                    take_snap(user_id,task_id);
                    write_data_to_json();
                    cout<<"Task title change successfully..."<<endl;
                }
                else{
                    cout<<"The combination of user id and task id is invalid..."<<endl;
                    return;
                }
            }
            void view_task(const string& user_id){
                if (user_exist(user_id) == false) {
                    cout << "Sorry the user does not exist..." << endl;
                    return;
                }

                auto it = user_task.find(user_id);
                if (it == user_task.end() || it->second.empty()) {
                    cout << "No tasks available for this user.\n";
                    return;
                }

                for (const auto& [task_id, curr] : user_task[user_id]) {
                    cout << "Task id     : " << curr.task_id << endl;
                    cout << "Task title  : " << curr.task_name << endl;
                    cout << "Task status : " << curr.task_status << endl;
                    cout << "Task note   : " << curr.task_note << endl;
                    cout << "---------------------------" << endl;
                }
            }




    };

}
vector<string> wk::User::task_status = {
    "TO-DO",
    "IN_PROGRESS",
    "COMPLETED"
};
unordered_map<string,unordered_map<string,string>> wk::Persistance::user_data; // {user_id : {user data...}}
unordered_map<string,unordered_map<string,snap>> wk::Persistance::user_task; //stores data for the tasks per user
unordered_map<string,unordered_map<string,vector<snap>>> wk::Persistance::history; // user_id : {task_id : {vector......}}

int main(){
    int choice;
    string name,gender,age,email,id,task_id,task_name,task_note,task_status;
    int stat_choice; //choice for the task status
    wk::Task t;
    t.restore_data();
    while(true){

        cout<<"\nWelcome to Pro Zone...."<<endl;
        cout<<"\n1.Add User \n2.Remove User \n3.Show all users \n4.Show a specific user \n5.Add task \n6.Remove task \n7.Change task title \n8.Change task status \n9.Change task note \n10.View Tasks for a user \n11.Show history \n12.Restore Task version \n13.Exit "<<endl;
        cout<<"Your action : ";
        cin>>choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if(choice == 1){
            cout<<"Enter your name : ";
            getline(cin,name);
            cout<<"Enter your gender : ";
            getline(cin,gender);
            cout<<"Enter your age : ";
            getline(cin,age);
            cout<<"Enter your mail address : ";
            getline(cin,email);
            cout<<"Enter a desired user id : ";
            getline(cin,id);
            t = wk::Task(name,gender,age,email,id);
        }
        else if(choice == 2){
            cout<<"Enter the user id : ";
            getline(cin,id);
            t.remove_user(id);
        }
        else if(choice == 3){
            t.view_all_users();
        }
        else if(choice == 4){
            cout<<"Enter user id to view : ";
            getline(cin,id);
            t.view_specific_user(id);

        }
        else if(choice == 5){
            cout<<"Enter your user id : ";
            getline(cin,id);
            cout<<"Enter a desired task id for your task : ";
            getline(cin,task_id);
            cout<<"Enter the task name : ";
            getline(cin,task_name);
            cout<<"Enter the status of the task | eg : todo, in_process, completed. : ";
            getline(cin,task_status);
            cout<<"Enter a task note : ";
            getline(cin,task_note);
            t.create_task(id,task_id,task_name,task_status,task_note);
        }
        else if(choice == 6){
            cout<<"Enter your user id : ";
            getline(cin,id);
            cout<<"Enter the task id you want to delete : ";
            getline(cin,task_id);
            t.remove_task(id,task_id);
        }
        else if(choice == 7){
            cout<<"Enter your user id : ";
            getline(cin,id);
            cout<<"Enter the task id : ";
            getline(cin,task_id);
            cout<<"Enter a new title for the task : ";
            getline(cin,task_name);
            t.change_task_title(id,task_id,task_name);
        }
        else if(choice == 8){
            cout<<"Enter your user id : ";
            getline(cin,id);
            cout<<"Enter the task id : ";
            getline(cin,task_id);
            cout<<"Enter which task status do you want : \n1.TO-DO \n2.IN_PROGRESS \n3.COMPLETED \nYour response : ";
            cin>>stat_choice;
            t.change_task_status(id,task_id,stat_choice);
        }
        else if(choice == 9){
            cout<<"Enter your user id : ";
            getline(cin,id);
            cout<<"Enter the task id : ";
            getline(cin,task_id);
            cout<<"Enter a new note for your task : ";
            getline(cin,task_note);
            t.change_task_note(id,task_id,task_note);
        }
        else if(choice == 10){
            cout<<"Enter your user id : ";
            getline(cin,id);
            t.view_task(id);
        }
        else if(choice == 11){
            cout<<"Enter your user id : ";
            getline(cin,id);
            cout<<"Enter the task id which you want to see history of : ";
            getline(cin,task_id);
            t.view_history(id,task_id);
        }
        else if(choice == 12){
            cout<<"Enter your user id : ";
            getline(cin,id);
            cout<<"Enter the task id you want to restore the version of : ";
            getline(cin,task_id);
            t.roll_back(id,task_id);
        }
        else if(choice == 13){
            cout<<"See you again.....\n";
            break;
        }
        else{
            cout<<"Please enter a valid option...\n";
        }

    }


    return 0;
}