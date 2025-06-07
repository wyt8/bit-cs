#include<iostream>
#include<vector>
#include<algorithm>
#include"string.h"
using namespace std;

/*  定义了4个实体类，分别是老师、学生、课程、成绩类，每个类中都有一个静态成员，是保存相应实体的集合；
*   每个实体类定义了相应的接口，用于支持查询本集合相关的信息。
*
*   定义了一个操作类，用于关联4个实体类，进行相应的操作。
*/

class Person;
class Teacher;
class Student;
class Course;
class Score;

class Person
{
public:
    Person(const char* name, bool gender, int year, int mouth, const char* ID):
    m_name(name), m_gender(gender), m_year(year),m_mouth(mouth), m_ID(ID)
    {}

protected:
    string m_name;
    // 男为0，女为1
    bool m_gender;
    int m_year;
    int m_mouth;
    string m_ID;
};

class Teacher: public Person
{
public:
    Teacher(const char* name, bool gender, int year, int mouth, const char* ID):
    Person(name, gender, year, mouth, ID)
    {// 每创建一个老师，就将其保存到TList中，TList是所有老师的集合
        TList.push_back(*this);
    }
    static string GetNameByID(string TID)
    {// 通过老师的ID获取老师的名字
        for(auto t: TList)
        {
            if(t.m_ID == TID)
            {
                return t.m_name;
            }
        }
        throw "can't find this Teacher!";
    } 
private:
    static vector<Teacher> TList;
};

class Student: public Person
{
public:
    Student(const char* name, bool gender, int year, int mouth, const char* ID):
    Person(name, gender, year, mouth, ID)
    {// 每创建一个学生，就将其保存到SList中，SList是所有课程的集合
        SList.push_back(*this);
    } 
    static Student& GetStudentByID(string SID)
    {// 通过学生的ID获取相应的学生类
        for(auto& s: SList)
        {
            if(s.m_ID == SID)
            {
                return s;
            }
        }
        throw "can't find this student!";
    }
private:
    static vector<Student> SList;
};

class Course
{
public:
    Course(string CID, string name, string TID): m_CID(CID), m_name(name), m_TID(TID)
    {// 每创建一个课程，就将其保存到CList中，CList是所有课程的集合
        CList.push_back(*this);
    }
    static string GetTeacherByID(string CID)
    {// 通过课程ID获取相应老师的ID
        for(auto c: CList)
        {
            if(c.m_CID == CID) {
                return c.m_TID;
            }
        }
        throw "can't find this course!";
    }
private:
    string m_TID;
    string m_CID;
    string m_name;
    static vector<Course> CList;
};

class Score
{
public:
    Score(string SID, string CID, int score): m_SID(SID), m_CID(CID), m_score(score)
    {// 每创建一个分数，就将其保存到RList中，RList是所有分数的集合
        RList.push_back(*this);
    }
    static int GetScore(string SID, string CID)
    {// 获得相应课程的分数
        for(auto r: RList)
        {
            if(r.m_SID == SID && r.m_CID == CID)
            {
                return r.m_score;
            }
        }
        return -1;
    }
    static void SetScore(string SID, string CID, int score)
    {// 设置相应课程的分数
        for(auto& r: RList)
        {
            if(r.m_SID == SID && r.m_CID == CID)
            {
                r.m_score = score;
            }
        }

    }
private:
    string m_SID;
    string m_CID;
    int m_score;
    static vector<Score> RList;
};

class Operation
{
public:
    string GetTeacherByCourse(string CID)
    {
        string TID = Course::GetTeacherByID(CID);
        return Teacher::GetNameByID(TID);
    }

    int GetScoreByCourse(string SID, string CID)
    {
        return Score::GetScore(SID, CID);
    }

    Student GetSInfoByID(string SID)
    {
        return Student::GetStudentByID(SID);
    }

    void SetScore(string SID, string CID, int score)
    {
        Score::SetScore(SID, CID, score);
    }
};



int main()
{
    system("pause");
    return 0;
}