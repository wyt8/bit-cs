#include<iostream>
#include<vector>
#include<algorithm>
#include"string.h"
using namespace std;

/*  ������4��ʵ���࣬�ֱ�����ʦ��ѧ�����γ̡��ɼ��࣬ÿ�����ж���һ����̬��Ա���Ǳ�����Ӧʵ��ļ��ϣ�
*   ÿ��ʵ���ඨ������Ӧ�Ľӿڣ�����֧�ֲ�ѯ��������ص���Ϣ��
*
*   ������һ�������࣬���ڹ���4��ʵ���࣬������Ӧ�Ĳ�����
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
    // ��Ϊ0��ŮΪ1
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
    {// ÿ����һ����ʦ���ͽ��䱣�浽TList�У�TList��������ʦ�ļ���
        TList.push_back(*this);
    }
    static string GetNameByID(string TID)
    {// ͨ����ʦ��ID��ȡ��ʦ������
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
    {// ÿ����һ��ѧ�����ͽ��䱣�浽SList�У�SList�����пγ̵ļ���
        SList.push_back(*this);
    } 
    static Student& GetStudentByID(string SID)
    {// ͨ��ѧ����ID��ȡ��Ӧ��ѧ����
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
    {// ÿ����һ���γ̣��ͽ��䱣�浽CList�У�CList�����пγ̵ļ���
        CList.push_back(*this);
    }
    static string GetTeacherByID(string CID)
    {// ͨ���γ�ID��ȡ��Ӧ��ʦ��ID
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
    {// ÿ����һ���������ͽ��䱣�浽RList�У�RList�����з����ļ���
        RList.push_back(*this);
    }
    static int GetScore(string SID, string CID)
    {// �����Ӧ�γ̵ķ���
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
    {// ������Ӧ�γ̵ķ���
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