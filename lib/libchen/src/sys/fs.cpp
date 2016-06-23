/**
 * Created by Jian Chen
 * @since  2016.01.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <chen/sys/fs.hpp>
#include <chen/base/str.hpp>
#include <sys/stat.h>
#include <cctype>

using namespace chen;

// -----------------------------------------------------------------------------
// path
std::string fs::drive(const std::string &path)
{
    auto sep = fs::separator(path);

    if (sep == '\\')
        return path.substr(0, 3);
    else if (!path.empty() && (path[0] == '/'))
        return "/";
    else
        return "";
}

char fs::separator(const std::string &path)
{
    // '\' if path is Windows path, otherwise use '/'
    if ((path.size() >= 3) && (path[2] == '\\') && (path[1] == ':') && std::isalpha(path[0]))
        return '\\';
    else
        return '/';
}

std::string fs::absolute(const std::string &path)
{
    if (fs::isRelative(path))
        return fs::current() + fs::separator() + fs::normalize(path);
    else
        return fs::normalize(path);
}

std::string fs::normalize(const std::string &path)
{
    if (path.empty())
        return "";

    auto abs = fs::drive(path);
    auto sep = fs::separator(path);

    std::size_t ptr = 0;  // current segment cursor
    std::size_t len = 0;  // current segment length

    std::vector<std::pair<std::size_t, std::size_t>> store;  // segments cache

    for (std::size_t i = abs.size(), l = path.size(); (i < l) || len; ++i)
    {
        if ((i == l) || (path[i] == sep))
        {
            if (len > 0)
            {
                // store a segment
                if (str::equal(&path[ptr], len, "..", 2))
                {
                    if (!store.empty() && !str::equal(&path[store.back().first], store.back().second, "..", 2))
                        store.pop_back();
                    else
                        store.emplace_back(std::make_pair(ptr, len));
                }
                else if (!str::equal(&path[ptr], len, ".", 1))
                {
                    store.emplace_back(std::make_pair(ptr, len));
                }

                ptr = len = 0;
            }
        }
        else
        {
            if (!len)
            {
                ptr = i;
                len = 1;
            }
            else
            {
                ++len;
            }
        }
    }

    // concat
    std::string ret(abs);

    for (std::size_t i = 0, l = store.size(); i < l; ++i)
    {
        auto &pair = store[i];
        ret.append(&path[pair.first], pair.second);

        if (i < l - 1)
            ret.append(1, sep);
    }

    return ret;
}

std::string fs::dirname(const std::string &path)
{
    if (path.empty())
        return "";

    auto abs = fs::drive(path);
    auto sep = fs::separator(path);

    auto beg = path.rbegin();
    auto end = path.rend() - abs.size();
    auto idx = beg;

    auto flag = false;

    for (; idx != end; ++idx)
    {
        if (*idx == sep)
        {
            auto next = idx + 1;

            if (flag && (*next != sep))
            {
                ++idx;
                break;
            }
        }
        else if (!flag)
        {
            flag = true;
        }
    }

    if (flag)
    {
        if (idx != end)
        {
            return path.substr(0, static_cast<std::size_t>(end - idx) + abs.size());
        }
        else
        {
            return !abs.empty() ? abs : ".";  // using root directory or current directory
        }
    }
    else
    {
        return abs;  // all chars are sep
    }
}

std::string fs::basename(const std::string &path)
{
    auto abs = fs::drive(path);
    auto sep = fs::separator(path);

    auto beg = path.rbegin();
    auto end = path.rend() - abs.size();

    auto flag_a = end;  // before
    auto flag_b = end;  // after

    for (auto idx = beg; idx != end; ++idx)
    {
        if (*idx == sep)
        {
            if (flag_b != end)
            {
                flag_a = idx;
                break;
            }
        }
        else if (flag_b == end)
        {
            flag_b = idx;
        }
    }

    if (flag_b != end)
        return path.substr(static_cast<std::size_t>(end - flag_a) + abs.size(), static_cast<std::size_t>(flag_a - flag_b));
    else
        return "";
}

std::string fs::extname(const std::string &path, std::size_t dots)
{
    if (!dots)
        return "";

    auto sep = fs::separator(path);
    auto beg = path.rbegin();
    auto end = path.rend();
    auto idx = beg;
    auto cur = beg;
    auto num = 0;

    for (; idx != end; ++idx)
    {
        if (*idx == sep)
        {
            break;
        }
        else if (*idx == '.')
        {
            cur = idx + 1;

            if (++num == dots)
                break;
        }
    }

    return num ? path.substr(static_cast<std::size_t>(end - cur), static_cast<std::size_t>(cur - beg)) : "";
}

// exist
bool fs::isAbsolute(const std::string &path)
{
    return (fs::separator(path) == '\\') || (!path.empty() && (path[0] == '/'));
}

bool fs::isRelative(const std::string &path)
{
    return !fs::isAbsolute(path);
}

// time
time_t fs::atime(const std::string &path)
{
    struct stat st = {0};
    return !::stat(path.c_str(), &st) ? st.st_atime : 0;
}

time_t fs::mtime(const std::string &path)
{
    struct stat st = {0};
    return !::stat(path.c_str(), &st) ? st.st_mtime : 0;
}

time_t fs::ctime(const std::string &path)
{
    struct stat st = {0};
    return !::stat(path.c_str(), &st) ? st.st_ctime : 0;
}

// size
off_t fs::filesize(const std::string &file)
{
    struct stat st = {0};
    return !::stat(file.c_str(), &st) ? st.st_size : 0;
}

// rename
bool fs::rename(const std::string &path_old, const std::string &path_new)
{
    // remove new path if it's already exist
    if (!fs::remove(path_new))
        fs::create(fs::dirname(path_new));  // create new directory

    // rename old path to new path
    return !::rename(path_old.c_str(), path_new.c_str());
}

// copy
bool fs::copy(const std::string &path_old, const std::string &path_new)
{
    if (fs::isFile(path_old))
    {
        auto folder = fs::dirname(path_new);
        if (folder.empty())
            return false;

        if (!fs::create(folder))
            return false;

        std::ifstream in(path_old, std::ios_base::binary);
        if (!in)
            return false;

        std::ofstream out(path_new, std::ios_base::binary);
        if (!out)
            return false;

        out << in.rdbuf();
        out.close();

        return true;
    }
    else if (fs::isDir(path_old, true))
    {
        if (!fs::create(path_new))
            return false;

        auto size = path_old.size();
        auto ret  = true;

        fs::visit(path_old, [size, &ret, path_new] (const std::string &name) -> bool {
            auto sub = name.substr(size, name.size() - size);

            if (fs::isFile(name))
                ret = fs::copy(name, path_new + sub);
            else
                ret = fs::create(path_new + sub, 0, false);

            // exit if occur an error
            return ret;
        });

        return ret;
    }
    else
    {
        return false;
    }
}

// visit
std::vector<std::string> fs::collect(const std::string &directory, bool recursive)
{
    std::vector<std::string> store;

    fs::visit(directory, [&store] (const std::string &path) -> bool {
        store.emplace_back(path);
        return true;
    }, recursive);

    return store;
}

// count
std::size_t fs::count(const std::string &directory,
                      bool recursive,
                      bool contain_file,
                      bool contain_dir)
{
    if (!contain_file && !contain_dir)
        return 0;

    std::size_t num = 0;

    fs::visit(directory, [&] (const std::string &path) -> bool {
        if ((contain_file && contain_dir) ||
            (contain_file && fs::isFile(path)) ||
            (contain_dir && fs::isDir(path)))
        {
            ++num;
        }

        return true;
    }, recursive);

    return num;
}

// read
std::string fs::read(const std::string &file)
{
    return fs::read(file, 0, fs::filesize(file));
}

std::string fs::read(const std::string &file, std::streamoff start, std::streamoff length)
{
    if (length <= 0)
        return "";

    std::ifstream in(file, std::ios_base::binary);

    if (start)
        in.seekg(start);

    if (in && !in.eof())
    {
        std::string ret(static_cast<std::size_t>(length), '\0');
        in.read(&ret[0], length);
        return ret;
    }

    return "";
}

std::vector<std::string> fs::read(const std::string &file, char delimiter)
{
    std::vector<std::string> ret;
    std::ifstream in(file, std::ios_base::binary);
    std::string line;

    while (std::getline(in, line))
        ret.emplace_back(std::move(line));

    return ret;
}

// write
bool fs::write(const std::string &file, const std::string &data, bool append)
{
    if (!fs::create(fs::dirname(file)))
        return false;

    std::ofstream out(file, std::ios_base::binary | (append ? std::ios_base::app : static_cast<std::ios_base::openmode>(0)));
    if (out)
        out.write(data.data(), data.size());

    return out.good();
}

bool fs::write(const std::string &file, const void *data, std::streamsize size, bool append)
{
    if (!fs::create(fs::dirname(file)))
        return false;

    std::ofstream out(file, std::ios_base::binary | (append ? std::ios_base::app : static_cast<std::ios_base::openmode>(0)));
    if (out)
        out.write(static_cast<const char*>(data), size);

    return out.good();
}