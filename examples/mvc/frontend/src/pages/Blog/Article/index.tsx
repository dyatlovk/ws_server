import { useEffect, useState } from 'react'
import { useParams } from 'react-router-dom'

const Article = (): JSX.Element => {
  const { id } = useParams()
  const [article, setArticle] = useState<Blog.Article>()
  const [isError, setError] = useState<boolean>(false)

  useEffect(() => {
    const data = getArticle(id)
    data.then(res => {
      if (!res) {
        setError(true)
      }
      setArticle(res)
    })
  }, [])

  return (
    <>
      {article && !isError && (
        <div>
          <h1>{article.title}</h1>
          <div dangerouslySetInnerHTML={{ __html: article.summary }}></div>
          <div dangerouslySetInnerHTML={{ __html: article.text }}></div>
        </div>
      )}

      {isError && <>error</>}
    </>
  )
}

async function getArticle(id: string): Promise<Blog.Article | null> {
  const response = await fetch(`https://srv.local/blog/${id}`, {
    method: 'GET',
  }).then(res => {
    if (!res.ok) {
      return null
    }
    return res
  })
  if (!response) return null
  const data = (await response.json()) as Blog.Article
  return data
}

export default Article
