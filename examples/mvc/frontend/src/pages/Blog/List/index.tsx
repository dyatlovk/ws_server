import { useEffect, useState } from 'react'
import { Link } from 'react-router-dom'

const List = (): JSX.Element => {
  const [list, setList] = useState<Blog.Article[]>([])

  useEffect(() => {
    const list = getList()
    list.then(res => setList(res))
  }, [])

  return (
    <div>
      {list.map((article, index) => {
        return (
          <div key={index}>
            <Link to={`/article/${article.id}`}>{article.title}</Link>
          </div>
        )
      })}
    </div>
  )
}

async function getList(): Promise<Blog.Article[]> {
  const response = await fetch(import.meta.env.VITE_BACKEND_HOST + "/blog", {
    method: 'GET',
  })

  const data = (await response.json()) as Blog.Article[]

  return data
}

export default List
